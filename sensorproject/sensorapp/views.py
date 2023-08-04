from django.shortcuts import render

import pyrebase

# Create your views here.
config = {
  'YOUR FIREBSE CONFIG' : 'FIREBASE_CONFIG'
}

firebase = pyrebase.initialize_app(config)
authe = firebase.auth()
database = firebase.database()

def index(request):
  all_json_data = database.child('reader').get().val()
  
  temp = database.child('current').child('temperature').get().val()
  hum = database.child('current').child('humidity').get().val()
  
  temp_total = getTotalTemperature(all_json_data)
  hum_total = getTotalHumidity(all_json_data)
  
  temp_avg = getAverageTemperature(all_json_data, temp_total)
  hum_avg = getAverageHumidity(all_json_data, hum_total)
  
  highest_temp = getHighestTemperature(all_json_data)
  highest_hum = getHighestHumidity(all_json_data)
  
  lowest_temp = getLowestTemperature(all_json_data)
  lowest_hum = getLowestHumidity(all_json_data)
  
  all_temp = getAllTemperatureValues(all_json_data)
  all_hum = getAllHumidityValues(all_json_data)
  

  context = {
      'tempCurrent': temp,
      'humCurrent': hum,
      'averageTemp': temp_avg,
      'averageHum': hum_avg,
      'maximumTemp': highest_temp,
      'maximumHum': highest_hum,
      'lowestTemp': lowest_temp,
      'lowestHum': lowest_hum,
      'tempData': all_temp,
      'humData': all_hum,
  }

  return render(request, 'index.html', context)

def getTotalTemperature(data) -> float:
  temp_total = 0
  for temp in data:
    temp_total += temp['temperature']

  return temp_total

def getTotalHumidity(data) -> float:
  hum_total = 0
  for hum in data:
    hum_total += hum['humidity']
    
  return hum_total
    
def getAverageTemperature(data, total_temp) -> float:
  average_temperature = total_temp / len(data)
  
  return round(average_temperature, 1)

def getAverageHumidity(data, total_hum) -> float:
  average_humidity = total_hum / len(data)
  
  return round(average_humidity, 1)

def getHighestTemperature(data) -> float:
  max_temp = float('-inf')
  for temp in data:
    temperature = temp['temperature']
    if temperature > max_temp:
      max_temp = temperature
      
  return max_temp
  

def getHighestHumidity(data) -> float:
  max_hum = float('-inf')
  for hum in data:
    humidity = hum['humidity']
    if humidity > max_hum:
      max_hum = humidity
      
  return max_hum

def getLowestTemperature(data) -> float:
  lowest_temp = float('inf')
  for temp in data:
    temperature = temp['temperature']
    if temperature < lowest_temp:
      lowest_temp = temperature
      
  return lowest_temp
  

def getLowestHumidity(data) -> float:
  lowest_hum = float('inf')
  for hum in data:
    humidity = hum['humidity']
    if humidity < lowest_hum:
      lowest_hum = humidity
      
  return lowest_hum

def getAllTemperatureValues(data):
  temperature_values = []
  for temp in data:
    temperature_values.append({ 'label': temp['time'], 'y':temp['temperature']})
    
  
  return temperature_values

def getAllHumidityValues(data):
  humidity_values = []
  for hum in data:
    humidity_values.append({ 'label': hum['time'], 'y':hum['humidity']})
    
  
  return humidity_values