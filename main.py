from flask import Flask

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def hello_world():
    print('Hello, World!')
    return 'Hello, World!'

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)