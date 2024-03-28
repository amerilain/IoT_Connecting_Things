const express = require('express');
const mqtt = require('mqtt');
const path = require('path');
const util = require('util');
const fs = require('fs');
const bodyParser = require('body-parser');
const app = express();

const readFile = util.promisify(fs.readFile);
const writeFile = util.promisify(fs.writeFile);

// Create variables for MQTT use here
app.use(bodyParser.json());
function read(filePath = './message.json') {
    return readFile(path.resolve(__dirname, filePath), 'utf8').then(data => {
        if (data) {
            try {
                return JSON.parse(data);
            } catch (error) {
                console.error('Error parsing JSON from file:', error);
                // If error in parsing JSON, return an empty array or default value
                return [];
            }
        } else {
            // If file is empty, return an empty array
            return [];
        }
    });
}
function write(data, filePath = './message.json') {
    return writeFile(path.resolve(__dirname, filePath), JSON.stringify(data));
}

// create an MQTT instance / variables for MQTT use
const mqttClient = mqtt.connect('mqtt://18.198.188.151:21883');

// Check that you are connected to MQTT and subscribe to a topic (connect event)
mqttClient.on('connect', () => {
    console.log('Connected to MQTT broker');
    mqttClient.subscribe('Yo/Mama', () => {
        console.log('Subscribed to topic');
    });
});

// handle instance where MQTT will not connect (error event)
mqttClient.on('error', (error) => {
    console.error('Cannot connect to MQTT:', error);
});

// Handle when a subscribed message comes in (message event)
mqttClient.on('message', async (topic, messageBuffer) => {
    const messageString = messageBuffer.toString();
    console.log(`Received message on ${topic}: ${messageString}`);

    try {
        const messages = await read();

        // Check if the message is already in our list to avoid duplicates
        const messageExists = messages.some(
            (m) => m.topic === topic && m.msg === messageString
        );

        if (!messageExists) {
            const newMessage = {
                id: messages.length + 1,
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
// Route to show a selected message by id
app.get('/:id', async (req, res) => {
    // Ensure the request is for a message detail, not other routes
    if (!isNaN(req.params.id)) { // Check if `id` is a number
        try {
            const messages = await read();
            const messageId = parseInt(req.params.id, 10); // Convert the id from string to integer
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
    } else {
        // If `id` is not a number, it might be a request for another route, so continue to next route handler
        next();
    }
});


// Route to CREATE a new message on the server and publish to mqtt broker
app.post('/', async (req, res) => {
    const { topic, msg } = req.body; // Assuming the body contains 'topic' and 'msg'
    try {
        const messages = await read();
        messages.push({ id: messages.length + 1, ...req.body });
        await write(messages);
        mqttClient.publish(topic, msg);
        res.sendStatus(201); // Successfully created
    } catch (error) {
        console.error('Error publishing message:', error);
        res.sendStatus(500); // Server error
    }
});

// Route to delete a message by id (Already done for you)
app.delete('/:id', async (req, res) => {
    try {
        const messages = await read();
        // Ensure that ID from params is treated as a number, since JSON does not distinguish between string and number types
        const messageId = parseInt(req.params.id, 10);
        const filteredMessages = messages.filter(message => message.id !== messageId);
        // Write the filtered messages back to the file
        await write(filteredMessages);
        res.sendStatus(200); // Send OK status
    } catch (error) {
        console.error('Error deleting message:', error);
        res.sendStatus(500); // Send internal server error status
    }
});


// listen to the port
app.listen(3000);