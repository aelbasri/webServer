import requests
img = "./tswira.png"

content = ''
with open(img, 'rb') as f:
    content = f.read()

length = len(content)

res = requests.post(url='http://localhost:3000',
                    data=content,
                    headers={'Content-Type': 'application/octet-stream', 'Content-Length': str(length)})
print(res.content)

