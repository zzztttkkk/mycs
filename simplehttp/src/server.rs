use std::{io, mem, net, sync, thread};
use std::collections::HashSet;
use std::sync::{Arc, Mutex, RwLock};
use std::time::Duration;
use crate::conn;

pub struct ServerInternal {
	host: String,
	port: u16,
	running: bool,
	stop: bool,
	closed: bool,
	listener: Option<Box<net::TcpListener>>,
	alive: HashSet<Arc<conn::Conn>>,
}

impl ServerInternal {
	pub fn new(host: String, port: u16) -> Self {
		return Self {
			host,
			port,
			stop: false,
			running: false,
			closed: false,
			listener: None,
			alive: HashSet::new(),
		};
	}

	pub fn listen(&mut self) {
		let ptr = Box::new(
			net::TcpListener::bind(format!("{}:{}", self.host, self.port)).unwrap()
		);
		self.listener = Some(ptr);
	}

	pub fn close(&mut self) -> Result<bool, u8> {
		self.closed = true;
		return Ok(true);
	}

	pub fn remove(&mut self, conn: &conn::Conn) {
		self.alive.remove(conn);
	}
}


pub struct Server {
	internal: Arc<RwLock<ServerInternal>>,
}

impl Server {
	pub fn new(host: String, port: u16) -> Self {
		return Self {
			internal: Arc::new(RwLock::new(ServerInternal::new(host, port))),
		};
	}

	pub fn close(&mut self) {
		let mut internal = self.internal.write().unwrap();
		if internal.stop { return; }
		internal.stop = true;
		mem::drop(internal);

		loop {
			let mut internal = self.internal.write().unwrap();
			if internal.closed {
				break;
			}
			thread::sleep(Duration::from_millis(500));
		}
	}

	pub fn running(&self) -> bool {
		let internal = self.internal.read().unwrap();
		return internal.running;
	}

	pub fn run(&mut self) {
		let mut internal = self.internal.write().unwrap();
		internal.listen();
		internal.listener.as_ref().unwrap().set_nonblocking(true);
		internal.running = true;
		println!("server running @ {}:{}", internal.host, internal.port);
		mem::drop(internal);

		loop {
			let internal = self.internal.read().unwrap();
			if internal.stop {
				mem::drop(internal);
				let mut internal = self.internal.write().unwrap();
				internal.running = false;
				internal.close();
				break;
			}
			mem::drop(internal);

			let mut internal = self.internal.write().unwrap();
			let incoming = internal.listener.as_ref().unwrap().accept();
			match incoming {
				Ok(stream) => {
					let conn = Arc::new(conn::Conn::new(self.internal.clone(), stream.0));
					internal.alive.insert(conn.clone());
					thread::spawn(move || {
						conn.handle();
					});
				}
				Err(e) => {
					if e.kind() != io::ErrorKind::WouldBlock {
						println!("{:?}", e);
					}
				}
			}
		}
	}
}