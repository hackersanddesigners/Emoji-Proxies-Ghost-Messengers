require( 'dotenv' ).config()


// We get the MQTT host and topic variables from our .env;
// and we initialize the mqtt object. This object contains
// the functions we need to interact with the HARDWARE in the
// space. The hardware send and receivbe data to and from this
// server by means of the MQTT object (and protoool)

const mqtt = require( './mqtt' )({
  HOST  : process.env.MQTT_HOST,
  TOPIC : process.env.MQTT_TOPIC,
})


// We set up a basic http server to handle serving our simple
// frontend. This will only server the static files when they
// are requested as well as the socket client.

const http = require( './http' )


// We initiatilize the WebSocket server with the http server
// and mqtt objects as parameters. This is the server that
// allows us to communicate with the online participants by
// means of live networking functions that send and receive
// data to and from their browsers.

const io = require( './io' )( http, mqtt )


// If either of the three were not initialized properly,
// we stop here and return an error. MQTT is optional

if ( !http || !io || !mqtt ) {
  throw new Error('HTTP, IO or MQTT not properly initialized.')
}




http.listen( process.env.HTTP_PORT )
