use std::sync::Mutex;

#[derive(Clone, Copy, Debug)]
pub enum LogLevel {
    Ok,
    Fail,
}

#[derive(Clone, Debug)]
pub struct LogEntry {
    pub level: LogLevel,
    pub label: &'static str,
    pub msg: String,
    pub ts: u64,
}

pub struct CircularBuffer {
    entries: [Option<LogEntry>; 256],
    head: usize,
}

impl CircularBuffer {
    const fn new() -> Self {
        const fn none_entry() -> Option<LogEntry> {
            None
        }
        CircularBuffer {
            entries: [const { None }; 256],
            head: 0,
        }
    }

    fn push(&mut self, entry: LogEntry) {
        self.entries[self.head] = Some(entry);
        self.head = (self.head + 1) % 256;
    }
}

static LOG_BUFFER: Mutex<CircularBuffer> = Mutex::new(CircularBuffer::new());

pub fn log_ok(label: &'static str, detail: &str) {
    let mut buffer = LOG_BUFFER.lock().unwrap();
    buffer.push(LogEntry {
        level: LogLevel::Ok,
        label,
        msg: detail.to_string(),
        ts: 0,
    });
    log::info!("[OK] {}: {}", label, detail);
}

pub fn log_fail(label: &'static str, reason: &str) {
    let mut buffer = LOG_BUFFER.lock().unwrap();
    buffer.push(LogEntry {
        level: LogLevel::Fail,
        label,
        msg: reason.to_string(),
        ts: 0,
    });
    log::info!("[FAIL] {}: {}", label, reason);
}
