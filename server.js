const express = require('express');
const app = express();
const serialport = require('serialport');
const server = require('http').createServer(app)
const io = require('socket.io')(server, {cors: {origin: '*'}});


app.use(express.static(__dirname + '/'));

app.get("/", (req, res) => {
    res.render('index')

})

const port = new serialport('COM4', {
	baudRate: 9600
});

io.on('connection', (socket) => {
    console.log('Estás conectado con el id:' + socket.id)

    port.on('data', (data) => {
        console.log(data);
        socket.emit('dataSending', data.toString('utf8'))
    })

    port.on('err', (err) => {
        console.log('Se produció un error')
    })
})


server.listen(3000, () => {
    console.log('Conexión exitosa')
})