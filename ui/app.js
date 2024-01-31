// app.js

// 添加日记条目
function addEntry() {
    // 获取用户输入的数据
    var entry = document.getElementById('entry').value;
    var priority = document.getElementById('priority').value;

    // 发送数据到后端（使用异步请求，例如 AJAX）
    // 这里假设有一个名为 addEntryBackend 的后端函数，你需要根据实际情况修改
    fetch('/addEntryBackend', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({
            entry: entry,
            priority: priority,
        }),
    })
    .then(response => response.json())
    .then(data => {
        // 处理后端返回的数据
        console.log(data);
        // 刷新页面或更新界面等操作
    })
    .catch(error => {
        console.error('Error:', error);
    });
}

// 查看日记条目
function viewEntries() 
{
    // 发送请求到后端，获取日记数据
    fetch('/viewEntriesBackend')
        .then(response => response.json())
        .then(data => {
            // 处理后端返回的数据
            console.log(data);
            // 刷新页面或更新界面等操作
        })
        .catch(error => {
            console.error('Error:', error);
        });
}

// 删除日记条目
function deleteEntry(entryId) 
{
    // 发送请求到后端，删除指定 ID 的日记条目
    fetch('/deleteEntryBackend', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({
            id: entryId,
        }),
    })
        .then(response => response.json())
        .then(data => {
            // 处理后端返回的数据
            console.log(data);
            // 刷新页面或更新界面等操作
        })
        .catch(error => {
            console.error('Error:', error);
        });
}
