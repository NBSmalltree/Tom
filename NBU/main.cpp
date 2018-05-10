#include"stdio.h"
#include"stdlib.h"


int n, m;
struct A
{
	int	a, b;
	int len;
}eage[20011];
int set[1011];


int cmp(const void *a, const void *b)
{
	struct A *c, *d;
	c = (struct A *)a;
	d = (struct A *)b;
	return d->len - c->len;
}


void build(int num)
{
	int i;
	for (i = 1; i <= num; i++)	set[i] = i;
}
int find(int k)
{
	if (set[k] == k)	return k;
	set[k] = find(set[k]);
	return set[k];
}
void Union(int f1, int f2)
{
	set[f1] = f2;
}


int Kruskal()
{
	int i;
	int ans;
	int count;
	int f1, f2;


	ans = 0;
	count = 1;
	for (i = 0; i < m; i++)
	{
		if (count == n)	break;
		f1 = find(eage[i].a);
		f2 = find(eage[i].b);
		if (f1 == f2)	continue;
		ans += eage[i].len;
		Union(f1, f2);
	}
	return ans;
}


int main()
{
	int i;
	int z;
	int ans;
	int T;

	scanf("%d", &T);

	while(T--){

		scanf("%d%d", &n, &m);
	
		build(n);


		for (i = 0; i < m; i++) { 
			scanf("%d%d%d", &eage[i].a, &eage[i].b, &eage[i].len); 
			eage[i].a++;
			eage[i].b++;
		}
		qsort(eage, m, sizeof(eage[0]), cmp);


		ans = Kruskal();


		z = 0;
		for (i = 1; i <= n; i++)	if (set[i] == i)	z++;
		if (z > 1)	printf("-1\n");
		else	printf("%d\n", ans);
	}
	return 0;
}