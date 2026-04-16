int a, b, c, i, n, sum;

a = 5;
b = 10;
c = 2;
i = 0;
n = 5;
sum = 0;

while (i < n)
{
    if (a < b)
    {
        sum = sum + (a + c);
    }
    else
    {
        sum = sum + (b * c);
    }

    i = i + 1;
}