#!/usr/bin/env python

import inspect
import msgpack
import socket
import struct
import subprocess
import sys
import time
import unittest

def loads(*args, **kwargs):
  kwargs['encoding'] = 'utf8'
  return msgpack.loads(*args, **kwargs)

def dumps(*args, **kwargs):
  kwargs['use_bin_type'] = True
  return msgpack.dumps(*args, **kwargs)

def read(path):
  with open(path, 'rb') as f:
    return f.read()


class VM:
  def __init__(self):
    self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.s.bind(('127.0.0.1', 0))
    self.port = self.s.getsockname()[1]
    self.s.listen()
    #self.sub = subprocess.Popen(["valgrind", "-v", "--leak-check=full", "./wanode", "-p", str(self.port)])
    self.sub = subprocess.Popen(["./wanode", "-p", str(self.port)])
    conn, addr = self.s.accept()
    self.c = conn
    self.seq = 0
    self.state = {}

    while True:
      seq, data = self.read()
      if data and None in data:
        v = data[None]
        if v == 'hello':
          print("Hello received with {0} v{1}".format(data['lang'], data['ver']))
          self.write_res(seq, {None: 'hello', 'lang': 'wasm', 'ver': 2})
          break
      else:
        continue

  def close(self):
    self.send_quit()
    self.sub.wait()
    self.c.close()
    self.s.close()

  def read(self):
    data = self.c.recv(4)
    if len(data) < 4:
      if len(data) > 0:
        raise Exception("Incomplete length")
      else:
        return None, None
    l, = struct.unpack('>I', data)
    l = l - 4
    data = self.c.recv(4)
    if len(data) < 4:
      if len(data) > 0:
        raise Exception("Incomplete seq")
      else:
        return None, None
    seq, = struct.unpack('>I', data)
    print("Got length header {0} seq {1}".format(l, seq))
    data = self.c.recv(l)
    print(data)
    data = loads(data, encoding='ascii')
    print(data)
    return seq, data

  def write(self, seq, data):
    data = dumps(data)
    self.c.sendall(struct.pack('>I', len(data)+4))
    self.c.sendall(struct.pack('>I', seq))
    self.c.sendall(data)

  def write_raw(self, seq, data):
    self.c.sendall(struct.pack('>I', len(data)+4))
    self.c.sendall(struct.pack('>I', seq))
    self.c.sendall(data)

  def write_req(self, data):
    self.seq += 1
    seq = self.seq << 1
    self.write(seq, data)

  def write_res(self, seq, data):
    seq = seq | 0x01
    self.write(seq, data)

  def send_quit(self):
    self.write_req({
      None: 'quit',
    })

  def make_ledger(self, bal = None, code=None):
    l = {}
    if bal:
      l['amount'] = bal
    else:
      l['amount'] = {b'SK': 666, b'TST': 777}
    if code:
      l['code'] = code
    if self.state:
      l['state'] = dumps(self.state)
    return l

  def make_tx(self, fr=b'fromaddr', to=b'totoaddr', kind=16, payload=[], call=None, code=None):
    t = {
      'f': fr,
      'to': to,
      'k': kind,
      'p': payload,
      's': 1,
      't': int(time.time())
    }
    if call:
      t['c'] = call

    if code:
      t['e'] = {'code': code}
    return t

  def send_tx(self, ledger, tx=None, call=None, gas=1000000000):
    r = {
      None: 'exec',
      'gas': gas,
      'ledger': dumps(ledger),
      'tx': dumps({'ver': 2, 'body': dumps(tx)})
    }
    if call:
      r['c'] = call

    self.write_req(r)
    seq, data = self.read()
    if 'state' in data:
      self.state = loads(data['state'])
    return data


class TestStringMethods(unittest.TestCase):
  def setUp(self):
    self.vm = VM()
    self.code = read("../rust/test-pair/target/wasm32-unknown-unknown/release/test_pair.wasm.lz4")

  def tearDown(self):
    self.vm.close()

  def test_exec(self):
    vm = self.vm
    vm.state = {}
    ret = vm.send_tx(
        vm.make_ledger(code=self.code),
        vm.make_tx( kind=18,
                    payload=[[1, "SK", 5000], [3, "GASK", 1000]],
                    call=['init', []],
                    code=self.code))

    ret = vm.send_tx(
        vm.make_ledger(code=self.code),
        vm.make_tx( kind=16,
                    payload=[[1, "SK", 5000], [3, "GASK", 1000]],
                    call=['save_data', [{"q1": "a1", "q2": "a2"}],],
                    code=self.code))

    ret = vm.send_tx(
        vm.make_ledger(code=self.code),
        vm.make_tx( kind=16,
                    payload=[[1, "SK", 5000], [3, "GASK", 1000]],
                    call=['save_data', [{"q1": "a2", "q2": "a2"}],],
                    code=self.code))

    self.assertEqual(ret[None], 'exec', 'Non-Exec reply')
    self.assertNotIn('err', ret, "Error in response")


    # ret = vm.send_tx(
        # vm.make_ledger({'SK': 1000}, code=self.code),
        # vm.make_tx( kind=16,
                    # payload=[[1, "SK", 5000], [3, "GASK", 1000]],
                    # call=['load_from_storage', []],
                    # code=self.code))
    # self.assertEqual(["asd", "zxc"], ret['ret'])



if __name__ == '__main__':
  unittest.main()
