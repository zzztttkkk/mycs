mod bubble;

pub enum SortMethod {
    Bubble,
}

impl SortMethod {
    pub fn sort<T: PartialOrd>(&self, ary: &mut [T]) {
        let less = |a: &T, b: &T| -> bool { return a.lt(b) };
        match self {
            SortMethod::Bubble => {
                bubble::sort(ary, less);
            }
        }
    }
}

pub static BUBBLE: SortMethod = SortMethod::Bubble;
