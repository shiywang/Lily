

int main()
{
	int a[12];
	int t1;
	int t2;
	t1 = t2;
	t2 = a[t1];
	a[t2] = t1;
	
	return 0;
}