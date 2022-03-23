use std::{mem, net, sync, thread};
use std::cell::RefCell;
use std::collections::HashSet;
use std::sync::{Arc, Mutex};
use crate::conn;

pub struct Server {
	host: String,
	port: u16,
	running: sync::Mutex<bool>,
	listener: Option<Box<net::TcpListener>>,
	alive: HashSet<Arc<Mutex<conn::Conn>>>,
}

impl Server {
	pub fn new(host: String, port: u16) -> Self {
		return Server {
			host,
			port,
			running: sync::Mutex::new(false),
			listener: None,
			alive: HashSet::new(),
		};
	}

	pub fn run(&mut self) {
		let mut running_guard = self.running.lock().unwrap();
		if *running_guard {
			return;
		}

		let ptr = Box::new(
			net::TcpListener::bind(format!("{}:{}", self.host, self.port)).unwrap()
		);
		self.listener = Some(ptr);

		*running_guard = true;
		mem::drop(running_guard);

		for stream in self.listener.as_ref().unwrap().incoming() {
			match stream {
				Ok(stream) => {
					let c = Arc::new(Mutex::new(conn::Conn::new(stream)));
					self.alive.insert(c.clone());
					thread::spawn(move || {
						let mut guard = c.lock().unwrap();
						guard.handle();
					});
				}
				Err(e) => {
					println!("{:?}", e);
				}
			}
		}
	}
}