int in[1920][1080];
int out[1920];
for(int i=0;i<1920;i++)
{

for(int j=0;j<1080;j++)
    {out[i]+=in[i][j];
    }
}
int minout;
minout=out[0];
for(int k=1;k<1920;k++)
{
    minout= minout<out[k]?minout:out[k];
}
minout+=12;
int right[1000];
int count=0;
for(int d=1918;d>0;d--)
{
    if (out[d]>minout){
        if (out[d+1]<minout){
            right[count]=d;
            count++;
        }
    }
}
count=0;
int left[1000];
for(int d=1918;d>0;d--)
{
    if (out[d]<minout){
        if (out[d+1]>minout){
            left[count]=d;
            count++;
        }
    }
}
int width[1000],widthsum=0;
for(int w=0;w<count;w++)
{
    width[w]=right[w]-left[w];
    widthsum+=width[w];
}
widthaver=widthsum/5;
for(int i=0;i<count;i++)
{
    if (width[i]<widthaver)
    {
        count--;
        for(int j=i;j<count;j++)
        {
            width[j]=width[j+1];
            left[j]=left[j+1];
            right[j]=right[j+1];

        }
    }
}
int finleft=left[0],finright=right[0];
for(int i=1;i<count;i++)
{
    finleft= finleft<left[i]? finleft:left[i];
    finright=finright>right[i]?finright:right[i];
}
