const express = require('express');
const mongoose = require('mongoose');
const bodyParser = require('body-parser');

const app = express();
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

mongoose.connect('mongodb://127.0.0.1/mydatabase', { useNewUrlParser: true, useUnifiedTopology: true });
const db = mongoose.connection;
db.on('error', console.error.bind(console, 'MongoDB connection error:'));

const mySchema = new mongoose.Schema({
  state1: String,
  state2: String,
  state3: String,
  state4: String
});

const MyModel = mongoose.model('MyModel', mySchema);

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/index.html');
});

app.post('/submit', (req, res) => {
  const {state1,state2,state3,state4 } = req.body;


  MyModel.findOneAndUpdate({}, {state1,state2,state3,state4}, { upsert: true ,new: true})
    .then(data => {
      res.send('Data saved to MongoDB');
    })
    .catch(err => {
      console.error(err);
      res.status(500).send('Error saving data to MongoDB');
    });
});


app.get('/data', (req, res) => {
  MyModel.findOne({})
	.then(data => {
		res.json(data);
	})
    .catch(err => {
      console.error(err);
      res.status(500).send('Error retrieving data from MongoDB');
    }); 
  });

// Start the server
app.listen(8000, () => {
  console.log('Server listening on port 3000');
});
