from flask import Flask, jsonify, request
import pandas as pd
from datetime import datetime

app = Flask(__name__)

# Define the path to the CSV file
csv_file_path = 'sensor_data.csv'

# Ensure the CSV file exists and has the correct columns
columns = ['time', 'motion', 'temp', 'hum']
try:
    df = pd.read_csv(csv_file_path)
except FileNotFoundError:
    df = pd.DataFrame(columns=columns)
    df.to_csv(csv_file_path, index=False)

@app.route('/', methods=['GET', 'POST'])
def get_light_status():
    if request.method == 'GET':
        response = {"message": "This is the response from the server"}
        return jsonify(response)
    elif request.method == 'POST':
        data = request.get_json()
        print("Received POST data:", data)
        
        # Validate the incoming data
        if all(key in data for key in ['temp', 'hum', 'motion']):
            # Add the current timestamp to the data
            data['time'] = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            
            # Append the new data to the CSV file
            new_data = pd.DataFrame([data], columns=columns)
            new_data.to_csv(csv_file_path, mode='a', header=False, index=False)
            response = {"message": "POST request received and data appended", "received_data": data}
        else:
            response = {"message": "Invalid data format", "received_data": data}

        return jsonify(response)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
