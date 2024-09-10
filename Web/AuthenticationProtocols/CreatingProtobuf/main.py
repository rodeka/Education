import json
import user_pb2
import requests

url = 'https://ac96b2c9-06ec-4f98-9aba-9bfadd791e1e-web.web.lms.itmo.xyz/'

json_data = {
    "id": "e1e8efe6-5dba-4043-ab6c-806772cada68",
    "name": "Lauren Johnson",
    "email": "mary80@example.org",
    "age": 47,
    "is_active": True,
    "address": {
        "street": "170 Brenda Harbors",
        "city": "Gillespieland",
        "state": "Kentucky",
        "zip_code": "20269",
        "country": "Pakistan"
    },
    "tags": [
        "mission",
        "may",
        "none",
        "choose",
        "skin"
    ]
}

# Создайте объект User и заполните его данными из JSON
user = user_pb2.User()
user.id = json_data["id"]
user.name = json_data["name"]
user.email = json_data["email"]
user.age = json_data["age"]
user.is_active = json_data["is_active"]

# Заполните объект Address
address = user.address
address.street = json_data["address"]["street"]
address.city = json_data["address"]["city"]
address.state = json_data["address"]["state"]
address.zip_code = json_data["address"]["zip_code"]
address.country = json_data["address"]["country"]

user.tags.extend(json_data["tags"])

serialized_data = user.SerializeToString()

headers = {'Content-Type': 'application/octet-stream'}
response = requests.post(url + '/validate', data=serialized_data, headers=headers)

print(response.text)
