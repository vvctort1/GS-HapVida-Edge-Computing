import requests
import matplotlib.pyplot as plt
from datetime import datetime
import matplotlib.dates as mdates

# Input do usuário para a quantidade de últimos dados a serem exibidos
num_data_points = int(input("Digite a quantidade de últimos dados que você quer exibir: "))

# Seu código para fazer a solicitação GET
url = f"http://46.17.108.113:8666/STH/v1/contextEntities/type/smartComfort/id/urn:ngsi-ld:smartComfort:001/attributes/position?lastN={num_data_points}"
headers = {'fiware-service': 'smart', 'fiware-servicepath': '/'}
response = requests.get(url, headers=headers)
data = response.json()

# Extrair os dados relevantes do JSON
values = data['contextResponses'][0]['contextElement']['attributes'][0]['values']

# Garantir que o número inserido não seja maior do que o número total de dados disponíveis
num_data_points = min(num_data_points, len(values))

# Filtrar os últimos dados conforme especificado pelo usuário
values = values[-num_data_points:]

# Extrair datas e valores
dates = [datetime.strptime(value['recvTime'], "%Y-%m-%dT%H:%M:%S.%fZ") for value in values]
position_values = [value['attrValue'] for value in values]

# Criar o gráfico
plt.figure(figsize=(10, 6))
plt.plot(dates, position_values, marker='o')
plt.title('Dados de Posição')
plt.xlabel('Data e Hora')
plt.ylabel('Posição')
plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d %H:%M:%S'))
plt.gca().xaxis.set_major_locator(mdates.AutoDateLocator())
plt.gcf().autofmt_xdate()
plt.show()
