from flask import Flask, render_template
import os

app = Flask(__name__, template_folder='templates')

@app.route('/')
def hello():
    template_path = os.path.join('index.html')
    return render_template(template_path, message='Hello, World! This is a simple HTTP server.')

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=8080, debug=True)
