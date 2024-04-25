const express = require('express');
const mqtt = require('mqtt');
const path = require('path');
const util = require('util');
const fs = require('fs');
const bodyParser = require('body-parser');
const app = express();
const { v4: uuidv4 } = require('uuid');
const options = {
    username: 'SmartIoT',
    password: 'SmartIoTMQTT'
};

const readFile = util.promisify(fs.readFile);
const writeFile = util.promisify(fs.writeFile);

// Create variables for MQTT use here
app.use(bodyParser.json());
function read(filePath = './message.json') {
    return readFile(path.resolve(__dirname, filePath)).then(data => JSON.parse(data));
}

function write(data, filePath = './message.json') {
    return writeFile(path.resolve(__dirname, filePath), JSON.stringify(data));
}

// create an MQTT instance / variables for MQTT use
const mqttClient = mqtt.connect('mqtt://18.198.188.151:21883', options);
//const mqttClient = mqtt.connect('mqtt://localhost:1883');

// Check that you are connected to MQTT and subscribe to a topic (connect event)
mqttClient.on('connect', () => {
    console.log('Connected to MQTT broker');
    // Subscribe to both topics
    mqttClient.subscribe(['Yo/Mama', 'Yo/Papa'], () => {
        console.log('Subscribed to topics Yo/Mama and Yo/Papa');
    });
});

// handle instance where MQTT will not connect (error event)
mqttClient.on('error', (error) => {
    console.error('Cannot connect to MQTT:', error);
});


// Handle when a subscribed message comes in (message event)
mqttClient.on('message', async (topic, messageBuffer) => {
    const messageString = messageBuffer.toString('utf8');
    console.log(`Received message on ${topic}: ${messageString}`);

    try {
        const messages = await read();


        const messageExists = messages.some(m => m.msg === messageString && m.topic === topic);

        if (!messageExists) {
            const newMessage = {
                id: uuidv4(),
                topic: topic,
                msg: messageString
            };
            messages.push(newMessage);
            await write(messages);
            console.log('Message added to message.json');
        }
    } catch (error) {
        console.error('Error handling incoming MQTT message:', error);
    }
});


// Route to serve the home page
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'index.html'));
});

// route to serve the JSON array from the file message.json when requested from the home page
app.get('/messages', async (req, res) => {
    try {
        const messages = await read();
        res.json(messages);
    } catch (error) {
        res.status(500).send('Error reading messages');
    }
});

// Route to serve the page to add a message
app.get('/add', (req, res) => {
    res.sendFile(path.join(__dirname, 'message.html'));
});

//Route to show a selected message. Note, it will only show the message as text. No html needed
app.get('/:id', async (req, res) => {
    try {
        const messages = await read();
        const messageId = req.params.id;
        const message = messages.find(m => m.id === messageId);

        if (message) {
            res.json(message);
        } else {
            res.status(404).send('Message not found');
        }
    } catch (error) {
        console.error('Error reading message:', error);
        res.status(500).send('Error reading message');
    }
});


// Route to CREATE a new message on the server and publish to mqtt broker
app.post('/', (req, res) => {
    const { topic, msg } = req.body;

    try {
        mqttClient.publish(topic, msg);
        res.sendStatus(200);
    } catch (error) {
        console.error('Error publishing message:', error);
        res.sendStatus(500);
    }
});


// Route to delete a message by id (Already done for you)
app.delete('/:id', async (req, res) => {
    try {
        const messages = await read();
        await write(messages.filter(c => c.id !== req.params.id));
        res.sendStatus(200);
    } catch (e) {
        res.sendStatus(200);
    }
});


// listen to the port
app.listen(3000);