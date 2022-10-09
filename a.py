from OpenSSL import SSL
import socket

def verify_cb(conn, cert, errun, depth, ok):
  return True

server = 'mail.google.com'
port = 443
PROXY_ADDR = ("proxy.example.com", 3128)
CONNECT = "CONNECT %s:%s HTTP/1.0\r\nConnection: close\r\n\r\n" % (server, port)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(PROXY_ADDR)
s.send(CONNECT)
#print s.recv(4096)      

ctx = SSL.Context(SSL.SSLv23_METHOD)
ctx.set_verify(SSL.VERIFY_PEER, verify_cb)
ss = SSL.Connection(ctx, s)

ss.set_connect_state()
ss.do_handshake()
cert = ss.get_peer_certificate()
#print cert.get_subject()
ss.shutdown()
ss.close()
