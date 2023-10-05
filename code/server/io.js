// SOCKET.IO functions are here. We use WebSockets protocol
// to communicate with more robust servers and applications
// it allwos for complex data objects and is a standard live
// networking protocol used accross media and platforms.

module.exports = ( http, mqtt ) => {


  // our socket.io options

  const options = {
    cors: {
      origin: '*',
      methods: ['GET', 'POST'],
      credentials: false
    }
  }


  // We initialize socket.io server with our http server

  const io = require( 'socket.io' )(  http, options )


  // Dirty method to log the # of connected sockets

  function count( socket ) {
    return socket.client.conn.server.clientsCount
  }


  // Emoji proxy

  function emoji( emoji ) {
    io.emit( 'emoji', emoji )
  }


  // any other miscellaneous functions

  function misc( data ) {
    io.emit( 'misc', data )
  }


  // Live stuff !!!

  function connect( socket ) {

    socket.on( 'emoji', ( data ) => {
      emoji( data )
      if ( mqtt ) {
        mqtt.hmmosphere.handler( data )
      }
    })


  }


  // We attach the connect function to the socket connection
  // event as its handler.

  io.on( 'connection', connect )


  // We add the functions to the io objects for later use
  // (outside of this file)

  io.socket_count   = count
  io.socket_connect = connect
  io.socket_emoji   = emoji
  io.socket_misc    = misc



  // We return the io object for use elsewhere.

  return io

}
