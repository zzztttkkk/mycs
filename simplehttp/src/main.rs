mod server;
mod conn;

fn main() {
	let serv = server::Server::new(String::from("127.0.0.1"), 8596);
	serv.run();
}
