#include <iostream>
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

std::vector<char> vBuffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& Socket) {

	Socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code EC, std::size_t Length) {
			if(!EC) {
				std::cout << "\n\nRead " << Length << "Bytes\n\n";
				for (int i = 0; i < Length; i++) std::cout << vBuffer[i];
				GrabSomeData(Socket);
			}
		});
}


int main() {

	asio::error_code Ec;

	// platform interface
	asio::io_context context;
	asio::io_context::work idleWork(context);
	std::thread Thread = std::thread([&]() { context.run(); });

	// address endpoints
	asio::ip::tcp::endpoint EndPoint(asio::ip::make_address("93.184.216.34", Ec), 80);

	asio::ip::tcp::socket Socket(context);
	Socket.connect(EndPoint, Ec);

	if (!Ec) std::cout << "Connected!\n";
	else std::cout << "Failed Code: " << Ec.message() << "\n";

	if (Socket.is_open()) {

		GrabSomeData(Socket);
		std::string sRequest = 
			"GET /index.html HTTP/1.1/r/n"
			"Host: example.com\r\n"
			"Connection: close \r\n\r\n";

		Socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), Ec);

		//// Manual reading
		//Socket.wait(Socket.wait_read);
		//size_t bytes = Socket.available();
		//std::cout << "Bytes Available: " << bytes << "\n";

		//if (bytes > 0) {

		//	std::vector<char> vBuffer(bytes);
		//	Socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), Ec);

		//	for (auto& c : vBuffer) std::cout << c;
		//}
	}

	return 0;
}