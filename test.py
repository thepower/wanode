#!/usr/bin/env python

import msgpack
import socket
import struct
import subprocess

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
  def __init__(self, conn):
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

  def send_tx(self, kind, call):
    self.write_req( {
      None: 'exec',
      'gas': 1111111111,
      'ledger': dumps({'code': bytearray(read(CODE_PATH)), 'state': dumps(self.state)}),
      'tx': dumps({'ver': 2, 'body': dumps({
        'k': kind,
        'f': b'\x80\x00 \x00\x02\x00\x00\x03',
        'p': [[0, 'XXX', 10], [1, 'FEE', 20]],
        'to': b'\x80\x00 \x00\x02\x00\x00\x05',
        's': 5,
        't': 1530106238743,
        'c': call,
        'e': {'code': bytearray(read(CODE_PATH))}
    })})})
    seq, data = self.read()
    if 'state' in data:
      self.state = loads(data['state'])
    return data


def do_tests(conn):
  vm = VM(conn)

  vm.state = {}
  vm.send_tx(18, ["init", [8]])
  vm.send_tx(16, ["inc", [1]])
  vm.send_tx(16, ["inc1", []])
  vm.send_tx(16, ["dec", [3]])
  vm.send_quit()


if __name__ == '__main__':
  HOST = '127.0.0.1'  # Standard loopback interface address (localhost)

  import sys
  CODE_PATH = sys.argv[1]

  with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, 0))
    PORT = s.getsockname()[1]
    s.listen()
    subprocess.Popen(["./wanode","-h", HOST, "-p", str(PORT)])
    conn, addr = s.accept()
    with conn:
      print('Connected by', addr)
      do_tests(conn)
