use mycs::algs;

#[test]
fn test_bubble_sort() {
	let mut vec = vec![1, 3, 4, 2];
	algs::sorting::BUBBLE.sort(&mut vec);
	println!("{:?}", vec);
}
