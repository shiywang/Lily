
int a[12];

int xxx()
{
	return 0;
}

int sum(int a, int b)
{
	return a+b;
}

int main()
{
	int i;
	int j;

	i = 1;
	j = 12;
	while(i < j) 
	{
		a[i+j] = sum(i,j);
		i = i + 1;
	}
	
	return 0;
}