Thera's networking module is a mid-level abstraction for game networking. Its core purpose is to provide a sensible interface for sending arbitrary data primarily over UDP via a backend tailored specifically to game development.

As of 14/06/2022, while the networking provides functional wrapping of vanilla TCP and UDP, it does not provide additional UDP features such as reliability. This will be added in future. This system additionally only handles IPv6 currently.

# Connections
`Thera::Net::tcp::Connection` and `Thera::Net::Connection` represent socket connections for TCP and UDP respectively. In the former case, the connection represents a local socket that has successfully 'shaken hands' with a remote TCP listener. UDP connections, as a result of the protocol, do not guarantee a valid remote socket.
## Creating a connection
Both TCP and UDP connections contain a static `Create` function which returns a shared pointer to the new connection. Connections cannot (and should not) be created without this function.

Both protocols have an overload of `Create` with a signature similar to this, where `[protocol]` is replaced by either `tcp` or `udp` :
```cpp
std::shared_ptr<Connection> Create(const asio::ip::[protocol]::endpoint& local, const asio::ip::address& address, const asio::ip::port_type& port)
```
The TCP variant appends an `asio::error_code&` for error reporting, but otherwise provides the same functional interface. 
`local` defines the origin of the new connection. This endpoint is often (especially when outbound) created with `endpoint(asio::ip::[protocol]::v4(), 0)` which creates an endpoint with a local IP on any available port. Alternatively, the port can be specified though this is not recommended for outbound connections.
`address` is the target remote IPv4 address, and `port` is the remote port.

### Additional overloads
#### UDP factories
`Create(const endpoint& local, const endpoint& endpoint)`
Identical to the above but accepts an `endpoint` rather than address and port.
`Create(const port_type& port)`
Creates a connection that listens on the specified port (see [Listeners](#Listeners) below).
#### TCP factories
`Create(asio::ip::tcp::socket& socket, bool startActive)`
Primarily for internal use. Creates a connection from an existing socket. Optionally doesn't start processing data immediately.

## Listeners
Listeners are objects that receive but not send data broadly, usually across a single port value. TCP has a separate listener object for this (`Thera::Net::tcp::Listener`) while UDP uses a `Connection` that only takes a local port.
### TCP Listener
The TCP Listener, like the connection, has a static `Create` function with the signature:
```cpp
Create(const asio::ip::port_type& port, const std::function<void(std::shared_ptr<Connection>)>& onAccept)
```
`port` is the port to listen on and accept connections from.
`onAccept` is a delegate called upon successfully accepting a connection request. It passes the new TCP `Connection` object as a parameter. If `onAccept` is null or the `Connection` is otherwise not stored, it will be immediately destroyed.
### UDP Listener
The UDP Listener is created via the second overload under [[#UDP factories]], and is of the same type as the connections. As UDP does not require a handshake, the listener's job is not to accept new connections in the same way as TCP, but instead to listen for user defined packets.

For example, a UDP listener may listen for a manually sent packet with ID `Connect`, and then handle that accordingly - perhaps creating a new UDP connection with the original source of that data.
# Sending & receiving
## Sending data
All data is sent in the form of packets (`Thera::Net::Packet`) which contain are defined by an ID (`Thera::Net::PacketID`). This ID informs the receiving connection of the packet's contents, and of how to process it. Packets are created through their constructors, usually the signature `Packet(ID)` where `ID` can be either an enum constant castable to `PacketID` or a value of type `PacketID` directly.

Once a packet is created, data can be added to it via either the `<<` operator (For basic types) or the templated `Write` member method, which handles any types defined in `CCXSerialisation.h`.

Once finished writing data, the target connection(s) are passed the packet via the `connection.Send(packet)` method, which queues the packet for sending next time `connection.Flush()` is called. This reduces the number of sends and the size of the UDP/TCP headers as packets are automatically aggregated by the system.

## Receiving data
Once a connection has been created and activated it will begin passively listening for data from its remote counterpart on a separate thread. When data is received, it is automatically constructed into a packet and passed off to a packet handler if one exists for that ID.
### Packet handlers
Packet handlers are function pointers of the signature `Connection& source, Packet* packet` and can be registered with the networking system via the `Thera::Net::RegisterPacket` and `Thera::Net::tcp::RegisterPacket` functions, which take a `PacketID` or castable enum as their first parameter, and the associated PacketHandler as their second.

Once registered, the PacketHandler will be called for all incoming packets with the associated ID.
### Reading packets
The recommended method of reading packets is through the `Packet.GetReader()` method. This creates a `CCX::MemoryReader` from the packet's data, allowing it to be read similarly to a stream. The `MemoryReader` class stores the position and length of the underlying data, and allows access through its `Read<T>()` method. This method is similarly constrained by the specialisations defined in `CCXSerialisation.h`.

Each call to `Read` will progress the reader forward by `sizeof(T)`, meaning two sequential calls to `Read<char>()` will return different values.
# Integration without Thera
Although this system is built for Thera in paticular, it can be fairly trivially transplanted into any other project, as its only dependencies are `asio` and parts of the `CCX` utility library (Specifically `ConsoleLogging`,  `CCXEvent`, `CCXThread`, `CCXMemory`, and `CCXType`).