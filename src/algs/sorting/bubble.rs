pub fn sort<T: PartialOrd, F: Fn(&T, &T) -> bool>(ary: &mut [T], less: F) {
    let mut ordered;
    for i in 0..ary.len() {
        ordered = true;
        for j in 0..ary.len() - 1 - i {
            if less(&ary[j + 1], &ary[j]) {
                ary.swap(j + 1, j);
                ordered = false;
            }
        }
        if ordered {
            break;
        }
    }
}
