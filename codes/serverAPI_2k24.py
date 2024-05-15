from http.server import SimpleHTTPRequestHandler, HTTPServer
import json
import random


#json_string = '{"r1":' + str(r1)
#print (json.dumps(json_string))
#print (json_data)

# Trata as requisições
class CustomRequestHandler(SimpleHTTPRequestHandler):

    def do_OPTIONS(self):
        self.send_response(200, "ok")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, OPTIONS')
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()
    
    def do_GET(self):   

        path = self.path
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Connection','close');
        self.send_header('Access-Control-Allow-Methods', 'GET, OPTIONS')
        self.send_header("Content-type", "application/json")
        self.end_headers()

        
        r1 = round((random.random()+0.01)*1,2)
        r2 = round((random.random()+0.01)*0.75,2)
        r3 = round((random.random()+0.01)*0.1,2)
        r4 = round((random.random()+0.01)*0.12,2)
        r5 = round((random.random()+0.01)*0.8,2)
        r6 = round((random.random()+0.01)*1.2,2)
        r7 = round((random.random()+0.01)*0.15,2)
        r8 = round((random.random()+0.01)*0.15,2)
        r9 = round((random.random()+0.01)*1.5,2)
        r10 = round((random.random()+0.01)*0.08,2)

        data = {}
        data['r1'] = r1
        data['r2'] = r2
        data['r3'] = r3
        data['r4'] = r4
        data['r5'] = r5
        data['r6'] = r6
        data['r7'] = r7
        data['r8'] = r8
        data['r9'] = r9
        data['r10'] = r10

        self.wfile.write(json.dumps(data).encode())


         
host = '192.168.100.177'
port = 8080

# cria o servidor HTTP
server = HTTPServer((host, port), CustomRequestHandler)
try:
    print(f"Servidor rodando em http://{host}:{port}")
    server.serve_forever()
except Exception as e:
    if self.running == True:
        exception(e)
info("HTTP Server stopped")


