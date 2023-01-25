//only thing I ve done here is turning pythin to c
void sk(int beat) {
	Sleep(beat / 1000);
}
void c4(int beat) {

	Beep(261 - 300, beat);
}void cs4(int beat) {
	Beep(277 - 300, beat);
}void  d4(int beat) {
	Beep(293 - 300, beat);
}void ds4(int beat) {
	Beep(311 - 300, beat);
}void e4(int beat) {
	Beep(329 - 300, beat);
}void f4(int beat) {
	Beep(349 - 300, beat);
}void fs4(int beat) {
	Beep(370 - 300, beat);
}void g4(int beat) {
	Beep(392 - 300, beat);
}void gs4(int beat) {
	Beep(415 - 300, beat);
}void a4(int beat) {
	Beep(440 - 300, beat);
}void as4(int beat) {
	Beep(466 - 300, beat);
}void b4(int beat) {
	Beep(494 - 300, beat);
}void c5(int beat) {
	Beep(523 - 300, beat);
}void cs5(int beat) {
	Beep(550 - 300, beat);
}void d5(int beat) {
	Beep(587 - 300, beat);
}void ds5(int beat) {
	Beep(622 - 300, beat);
}void e5(int beat) {
	Beep(659 - 300, beat);
}void f5(int beat) {
	Beep(699 - 300, beat);
}void fs5(int beat) {
	Beep(740 - 300, beat);
}void g5(int beat) {
	Beep(784 - 300, beat);
}void gs5(int beat) {
	Beep(830 - 300, beat);
}void a5(int beat) {
	Beep(880 - 300, beat);
}void as5(int beat) {
	Beep(932 - 300, beat);
}void b5(int beat) {
	Beep(987 - 300, beat);
}void c6(int beat) {
	Beep(1047 - 300, beat);
}
void cs6(int beat) {
	Beep(1175 - 300, beat);
}
void Play() {

	int b = 200;

	b4(b);
	a4(b);
	gs4(b);
	a4(b);
	c5(b * 4);
	d5(b);
	c5(b);
	b4(b);
	c5(b);
	e5(b * 4);
	f5(b);
	e5(b);
	ds5(b);
	e5(b);
	b5(b);
	a5(b);
	gs5(b);
	a5(b);;
	b5(b);
	a5(b);
	gs5(b);
	a5(b);
	c6(b * 4);
	g5(350);
	a5(350);
	b5(350);
	a5(350);
	g5(350);
	a5(350);
	b5(350);
	a5(350);
	g5(350);
	a5(350);
	b5(350);
	a5(350);
	g5(b * 2);
	fs5(b * 2);
	e5(b * 4);
	sk(100);
	e5(300);
	f5(300);
	g5(300);
	g5(300);
	a5(150);
	g5(150);
	f5(150);
	e5(150);
	d5(b * 4);
	sk(100);
	e5(300);
	f5(300);
	g5(300);
	g5(300);
	a5(150);
	g5(150);
	f5(150);
	e5(150);
	d5(b * 4);
	c5(300);
	d5(300);
	e5(300);
	e5(300);
	f5(150);
	e5(150);
	d5(150);
	c5(150);
	b4(b * 4);
	sk(100);
	c5(300);
	d5(300);
	e5(300);
	e5(300);
	f5(150);
	e5(150);
	d5(150);
	c5(150);
	b4(b * 4);
	b4(b);
	a4(b);
	gs4(b);
	a4(b);
	c5(b * 4);
	d5(b);
	c5(b);
	b4(b);
	c5(b);
	e5(b * 4);
	f5(b);
	e5(b);
	ds5(b);
	e5(b);
	b5(b);
	a5(b);
	gs5(b);
	a5(b);
	b5(b);
	a5(b);
	gs5(b);
	a5(b);
	c6(b * 4);
	a5(b * 2);
	b5(b * 2);
	c6(b * 2);
	b5(b * 2);
	a5(b * 2);
	gs5(b * 2);
	a5(b * 2);
	e5(b * 2);
	f5(b * 2);
	d5(b * 2);
	c5(b * 4);
	b4(b * 4);
	a4(b * 8);
}
