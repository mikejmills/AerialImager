#!/usr/bin/python

import socket

'''s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
s.connect("/tmp/viewer_socket")
s.send('0\00')
data = s.recv(1024)
s.close()
print 'Received', repr(data)
'''

from wxPython.wx import *

"""Hello World - wxPython style"""

class Greeter(wxApp):
    def OnInit(self):
        frame = wxFrame(NULL, -1, "Hello wxPython world")
        frame.Show(true)
        self.SetTopWindow(frame)
        return true

firstapp = Greeter(0)
firstapp.MainLoop()