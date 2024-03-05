from flask import Flask, jsonify
import requests

app = Flask(__name__)

@app.route('/frontend')
def frontend():
    # 向 C++ 后端发送 HTTP 请求
    cpp_backend_url = 'http://localhost:8080/api/data'
    response = requests.get(cpp_backend_url)

    # 处理从 C++ 后端接收到的数据
    data_from_cpp = response.json()

    # 在前端进行处理并渲染
    return jsonify(data_from_cpp)

if __name__ == '__main__':
    app.run(debug=True, port=5000)
