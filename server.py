#!/usr/bin/env python

import msgpack
import socket
import socketserver
import struct
import threading
import base64

def loads(*args, **kwargs):
  kwargs['encoding'] = 'utf8'
  return msgpack.loads(*args, **kwargs)

def dumps(*args, **kwargs):
  kwargs['use_bin_type'] = True
  return msgpack.dumps(*args, **kwargs)

def read(path):
  with open(path, 'rb') as f:
    return f.read()


CODE_PATH = "test1.wasm"

class ThreadedTCPRequestHandler(socketserver.BaseRequestHandler):
  kseq = 0
  def read(self):
    data = self.request.recv(4)
    if len(data) < 4:
      if len(data) > 0:
        raise Exception("Incomplete length")
      else:
        return None, None
    l, = struct.unpack('>I', data)
    l = l - 4
    data = self.request.recv(4)
    if len(data) < 4:
      if len(data) > 0:
        raise Exception("Incomplete seq")
      else:
        return None, None
    seq, = struct.unpack('>I', data)
    print("Got length header {0} seq {1}".format(l, seq))
    data = self.request.recv(l)
    print(data)
    data = loads(data, encoding='ascii')
    print(data)
    return seq, data

  def write(self, seq, data):
    data = dumps(data)
    self.request.sendall(struct.pack('>I', len(data)+4))
    self.request.sendall(struct.pack('>I', seq))
    self.request.sendall(data)

  def write_raw(self, seq, data):
    self.request.sendall(struct.pack('>I', len(data)+4))
    self.request.sendall(struct.pack('>I', seq))
    self.request.sendall(data)

  def write_req(self, data):
    self.kseq += 1
    seq = self.kseq << 1
    self.write(seq, data)

  def write_res(self, seq, data):
    seq = seq | 0x01
    self.write(seq, data)


  def handle(self):
    print("Client connected")
    while True:
      seq, data = self.read()
      if data and None in data:
        v = data[None]
        if v == 'hello':
          print("Hello received with {0} v{1}".format(data['lang'], data['ver']))
          self.write_res(seq, {None: 'hello', 'lang': 'wasm', 'ver': 2})

          self.process()
        else:
          pass
          #print("Unknown message type")
      else:
        break

  def process(self):
    # self.write_raw(4, open('/tmp/vmproto_req_0.bin', 'rb').read())
    # self.write_raw(4, open('/tmp/vmproto_req_2.bin', 'rb').read())
    # self.write_raw(4, open('/tmp/vmproto_req_4.bin', 'rb').read())
    # self.send_quit()
    # return

    self.state = {}
    self.send_tx(18, ["init", [8]])
    self.send_tx(16, ["inc", [1]])
    self.send_tx(16, ["inc1", []])
    self.send_tx(16, ["dec", [3]])
    #self.send_tx(16, ["test", [512]])
    #self.send_tx(16, ["tt", [128, -128, True, b'\x01\x02\x03', b'\x01\x02\x03\x04\x05\x06\x07\x08']])
    self.send_quit()

  def send_tx(self, kind, call):
    self.write_req( {
      None: 'exec',
      'gas': 11111,
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
    if data:
      if 'err' in data:
        print("ERROR RECEIVED")
      else:
        print("RESPONSE: {0}".format(data))
        self.state = loads(data['state'])


  def send_quit(self):
    self.write_req({
      None: 'quit',
    })


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
  pass

if __name__ == '__main__':
  import sys
  if len(sys.argv) > 1:
    CODE_PATH = sys.argv[1]

  print("Using {0}".format(CODE_PATH))

  server = ThreadedTCPServer(("localhost",5555), ThreadedTCPRequestHandler)
  ip, port = server.server_address
  server.serve_forever()
