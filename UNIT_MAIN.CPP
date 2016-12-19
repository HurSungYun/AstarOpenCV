#include<cv.h>
#include <highgui.h>
#include"Priority_Queue.h"
#include "ASTAR.H"


#define IMAX 1400


RECT crt;

FILE *out2=fopen("test.txt","w");

UNIT *unit;
HDC Display=NULL;
HBITMAP Display_BITMAP;
HBITMAP BackGround;

int start_x=1;
int start_y=200;
int end_x, end_y;
int Width=700;//�ʱ� ������ �ʺ�
int Height=700;//�ʱ� ������ ����

int ASTAR_SW;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Start
typedef pair<int,int > Pair;

int imap[IMAX][IMAX];
int path[IMAX][IMAX];   // ��� ����.  1 : ���ʿ��� ��, 2 : �����ʿ��� ��, 3 : ���ʿ��� ��, 4 : �Ʒ��ʿ��� ��
int test[IMAX][IMAX];	// �׽�Ʈ�� �迭( ������ Ž���� ����)
int cnt[IMAX][IMAX];
int pt[4][2]={{1,0},{-1,0},{0,1},{0,-1}};
int n,m;
int x=-1,y=-1;

PQNode Popped;

int stx,sty,endx,endy; // �������� ����
IplImage* image;


CvCapture* m_pCapture;
CvvImage  m_cImage;
IplImage* m_pImage;

	int BlueX=0,BlueY=0;
	int GreenX=0,GreenY=0;


void translation(){
	int i,j;
	int r, g, b;
	int Y, Cr, Cb;
	int BlueSW=0, GreenSW=0;

	cvSmooth(m_pImage,m_pImage);
	IplImage* YCrCb = cvCloneImage(m_pImage);
	cvConvertImage(m_pImage,YCrCb,CV_BGR2YCrCb);
	for(i=0;i<m_pImage->height;i++)
		for(j=0;j<m_pImage->width; j++){
			Y = (unsigned char)YCrCb->imageData[i*YCrCb->widthStep+j*3];
			Cr = (unsigned char)YCrCb->imageData[i*YCrCb->widthStep+j*3+1];
			Cb = (unsigned char)YCrCb->imageData[i*YCrCb->widthStep+j*3+2];

			b = (unsigned char)m_pImage->imageData[i*m_pImage->widthStep+j*3];
			g = (unsigned char)m_pImage->imageData[i*m_pImage->widthStep+j*3+1];
			r = (unsigned char)m_pImage->imageData[i*m_pImage->widthStep+j*3+2];

			if(130<Y && b>100 && g<110 && r<110){	//BLUE
				m_pImage->imageData[i*m_pImage->widthStep+j*3] = 255;
				m_pImage->imageData[i*m_pImage->widthStep+j*3+1] = 0;
				m_pImage->imageData[i*m_pImage->widthStep+j*3+2] = 0;
				if(BlueSW == 0)
					BlueSW = 1, BlueX = j, BlueY = i;
			}else if(0<Y && Y<70 && g>100 && b<100 && r<100){	//GREEN
				m_pImage->imageData[i*m_pImage->widthStep+j*3] = 0;
				m_pImage->imageData[i*m_pImage->widthStep+j*3+1] = 255;
				m_pImage->imageData[i*m_pImage->widthStep+j*3+2] = 0;
				if(GreenSW == 0)
					GreenSW = 1, GreenX = j, GreenY = i;
			}else if(Y<130 && b<100 && g<100 && r<150){		//BLACK
				m_pImage->imageData[i*m_pImage->widthStep+j*3] = 0;
				m_pImage->imageData[i*m_pImage->widthStep+j*3+1] = 0;
				m_pImage->imageData[i*m_pImage->widthStep+j*3+2] = 0;
			}else{											//WHITE
				m_pImage->imageData[i*m_pImage->widthStep+j*3] = 255;
				m_pImage->imageData[i*m_pImage->widthStep+j*3+1] = 255;
				m_pImage->imageData[i*m_pImage->widthStep+j*3+2] = 255;
			}
		}


		cvReleaseImage(&YCrCb);
}

void convert()
{
	int i,j;
	int stcnt=0;
	int endcnt=0;
	int r, g, b;

	image = cvLoadImage("Bitmap1.bmp", 1);

	for(i=0;i<image->height;i++)
	{
		imap[i][0]=1;
		imap[i][image->width-1]=1;
	}
	for(i=0;i<image->width;i++)
	{
		imap[0][i]=1;
		imap[image->height-1][i]=1;
	}
	for(i=1;i<image->height-1;i++)
	{
		for(j=1;j<image->width-1;j++)
		{
			b = (unsigned char)image->imageData[image->widthStep*i+j*3];
			g = (unsigned char)image->imageData[image->widthStep*i+j*3+1];
			r = (unsigned char)image->imageData[image->widthStep*i+j*3+2];
			if(b==255 && g==0 && r==0)
			{
				imap[i][j]=3;
			}
			else if(b==0 && g==0 && r==255)
			{
				imap[i][j]=2;
			}
			else if(b==0 && g==0 && r==0)
			{
				imap[i][j]=1;
			}
			else if(b==255 && g==255 && r==255)
			{
				imap[i][j]=0;
			}
		}
	}

	n = image->height;
	m = image->width;


	for(i=0;i<n;i++)          // ���� ����ȭ��Ų ������ �����ϰ�, �ν��ϰ�, ������� �������� �ϳ��� ���ϴ� 2�� for��
	{
		for(j=0;j<m;j++)
		{
			if(imap[i][j]==2 && stcnt==0)
			{
				endx=j; 
				endy=i; 
				stcnt=1;
			}
			
			if(imap[i][j]==3 && endcnt==0)
			{
				stx=j;
				sty=i;
				endcnt=1;
			}
		}
	}
	cvReleaseImage(&image);

}


double g(int t[2])
{
	return (float)cnt[t[0]][t[1]];  // �̶����� ������ �Ÿ�
}

double h(int t[2])
{
//	return sqrt((float)(t[1]-endx)*(t[1]-endx)+(t[0]-endy)*(t[0]-endy)); //��Ÿ���� �Ÿ�
	return pow((double)fabsf(t[1]-endx)+fabsf(t[0]-endy),0.8);
}

double f(int t[2])
{
	return g(t)+h(t);  // ��ü
}

void init(PriorityQueue* Openlist)
{
	PQNode* stNode=(PQNode*)malloc(sizeof(PQNode));           //�������� openlist�� �ִ� ����
	Pair* stPair = (Pair*)malloc(sizeof(Pair));
	*stPair = make_pair(sty,stx);
	stNode->Data=stPair;
	stNode->Priority=0.000;
	 PQ_Enqueue(Openlist,*stNode);


	 int i,j;
	 for(i=0;i<IMAX;i++)
	 {
		 for(j=0;j<IMAX;j++)
		 {
			 cnt[i][j]=0;
			 path[i][j]=0;
		 }
	 }

}

int stack[IMAX*IMAX][3];
int top=0;


void moves_like_seokha()    //�κ��� �̵���Ų��.
{
	if(y==sty && x==stx)
		return;

	stack[top][0]=y;
	stack[top][1]=x;
	top++;

		switch(path[y][x])
		{
			case 2:	unit->PutV(0,1);
					y++;
					break;

			case 1: unit->PutV(0,-1);
					y--;
					break;

			case 4: unit->PutV(1,0);  // Ȥ�ó� ������ �̵��� ���� -1�� +1�� ��ü�غ�����. ���� �߸� �Է����� ���� ������
					x++;
					break;
			
			case 3: unit->PutV(-1,0);
					x--;
					break;
			default: unit->PutV(0,0);
		}
		unit->Move();
}

void primary_process()
{
	
	PriorityQueue* Openlist=PQ_Create(3);
	int i,j,t[2],k;

	convert();			//��Ʈ�� �̹����� �������� ��Ŵ

	if(x==-1 && y==-1)
	{
		x=endx;
		y=endy;
	}


	init(Openlist);  //openlist �ʱ�ȭ �� ���۰� �־���
	

	while(!PQ_IsEmpty(Openlist))       // �� �Լ��� ���� ����
	{
		PQ_Dequeue(Openlist, &Popped);
		t[0]=Popped.Data->first;
		t[1]=Popped.Data->second;					// openlist���� �ּҰ� ����

		if(t[0]==y && t[1]==x)				//���� endx,endy��� print()�Լ��� �����Ų��. !!!!��������!!!!
		{
			int x2=stx,y2=sty,t2,t3;
			for(i=0;i<n;i++)
			{
				for(j=0;j<m;j++)
				{
					if(i==endy && j==endx) fprintf(out2,"%d*",path[i][j]);
					else
					fprintf(out2,"%d ",path[i][j]);
				}
				fprintf(out2,"\n");
			}
			break;
		}
//		if(path[t[0]][t[1]]==0)
		{
			for(i=0;i<4;i++) // 4��������, f(x) ���� ����Ͽ� �̸� ���� ����Ʈ�� �ٽ� �ִ´�.
			{
				int t_cnt=cnt[t[0]][t[1]];
				t[0]+=pt[i][0];
				t[1]+=pt[i][1];
				if(t[0]>=0 && t[0]<n && t[1]>=0 && t[1]<m)
				{
					if(path[t[0]][t[1]]==0 &&  imap[t[0]][t[1]]!=1)
					{
					
						cnt[t[0]][t[1]]=t_cnt+1;
		
						PQNode* stNode=(PQNode*)malloc(sizeof(PQNode));			//openlist�� �־���
						Pair* stPair = (Pair*)malloc(sizeof(Pair));
		
						*stPair = make_pair(t[0],t[1]);
						stNode->Data=stPair;
						stNode->Priority=f(t);
						k=PQ_Refresh(Openlist, stNode); //k�� �ߺ� ���θ� ���ϰ�, refresh �ؾ��ϴ� �� ���ϴ� ����
					
						if(k==1)
							path[t[0]][t[1]]=i+1;
						else if(k==0)
						{	
						 PQ_Enqueue(Openlist,*stNode);
						 path[t[0]][t[1]]=i+1;
						}
					}
						
				}
				t[0]-=pt[i][0];
				t[1]-=pt[i][1];
			}
		}

	}
	PQ_Destroy(Openlist);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////ENd

void set_start_pt()
{
	image = m_pImage;

	start_x = GreenX;
	start_y = GreenY;
	return;
}

void Init_API(){
	BackGround = (HBITMAP)LoadImage(NULL,"Image\\map.bmp",NULL,0,0,LR_LOADFROMFILE);
//	BITMAP bmp;//��Ʈ�� �Ӽ�
//	GetObject(BackGround, sizeof(BITMAP), (LPVOID) &bmp);
//	Width  = (int)bmp.bmWidth;  // �׸��� ��
//	Height = (int)bmp.bmHeight; // �׸��� ����
	m_pCapture=cvCaptureFromCAM(0);

	m_pImage = cvQueryFrame(m_pCapture);

	Width = m_pImage->width;
	Height = m_pImage->height;

	ASTAR_SW = 0;
}
void Make_Unit(UNIT *Unit){
	set_start_pt();
	unit = new UNIT(start_x*crt.right/Width,start_y*crt.bottom/Height);
}

BOOL SaveDisplay(LPCSTR lpFileName){	//	Display_BITMAP�� �����Ѵ�.

	HBITMAP hBitmap = Display_BITMAP;
	
    BITMAP bitmap;                                  // ��Ʈ��(DDB) ����ü
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);    // ��Ʈ��(DDB)�� ������ ����
	
	
    BITMAPINFOHEADER bmih;                          // ��Ʈ��(DIB) ���� ��� ����ü
    ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = bitmap.bmWidth;
    bmih.biHeight = bitmap.bmHeight;
    bmih.biPlanes = 1;
    bmih.biBitCount = 24;       // 24��Ʈ Ʈ���÷�
    bmih.biCompression = BI_RGB;
	
	
	
    HDC hDC = GetDC(GetDesktopWindow());
	
    // ��Ʈ�� ������ ������ ����
    GetDIBits(hDC, hBitmap, 0, bitmap.bmHeight, NULL, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	
	
    // ��Ʈ�� �����͸� �����ϱ� ���� ���� ����
    LPBYTE lpBits = new BYTE[bmih.biSizeImage];
	
    // ��Ʈ�� �����͸� ������
    GetDIBits(hDC, hBitmap, 0, bitmap.bmHeight, lpBits, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	
    ReleaseDC(GetDesktopWindow(), hDC);
	
	
    // ���� ����
    HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        delete [] lpBits;
        return FALSE;
	}
	
	
	
    BITMAPFILEHEADER bmfh;  // ��Ʈ�� ���� ��� ����ü
    bmfh.bfType = 'MB';     // ��Ʈ�� ���� �ν���
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfh.bfSize = bmfh.bfOffBits + bmih.biSizeImage;
    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	
	
	
    DWORD dwWritten;
    WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);    // ��Ʈ��������� ����
    WriteFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);    // ��Ʈ��������� ����
    WriteFile(hFile, lpBits, bmih.biSizeImage, &dwWritten, NULL);           // ��Ʈ�ʵ����� ����
    CloseHandle(hFile);
	
	
	
    
	delete [] lpBits;
    return TRUE;  
}

void RealTime(){
	static int t_time=-1;
	if(t_time==0)	t_time = GetTickCount();

	if(ASTAR_SW==0){
		if(m_pCapture){
//			if(m_pImage)
//				cvReleaseImage(&m_pImage);
			m_pImage = cvQueryFrame(m_pCapture);
			translation();
		}
	}else{
		if(GetTickCount()-t_time>=3000){
			SaveDisplay("Bitmap1.bmp");
			if(t_time==-1)
			{
				primary_process();
			}
			t_time=0;
		}
		moves_like_seokha();
	}

}

void Print(HDC *hdc){
	HDC BackDC, hMemDC2;
	HBITMAP OldBitmap,TBitmap, OldBitmap2;
	HBRUSH hBrush, OldBrush,BGBrush;
	HPEN hPen, OldPen;
	hMemDC2 = CreateCompatibleDC(*hdc);
	TBitmap = CreateCompatibleBitmap(*hdc,Width,Height);
	OldBitmap2 = (HBITMAP)SelectObject(hMemDC2, TBitmap);
	BackDC = CreateCompatibleDC(*hdc);
	OldBitmap = (HBITMAP)SelectObject(BackDC, BackGround);
	BitBlt(hMemDC2,0,0,Width,Height,BackDC,0,0,SRCCOPY);

		RECT rt;
		rt.left = crt.left;
		rt.right = crt.right;
		rt.top = crt.top;
		rt.bottom = crt.bottom;

	if(m_pImage){
		m_cImage.CopyOf(m_pImage);
		m_cImage.DrawToHDC(hMemDC2,&rt);
	}else{
		BGBrush = CreateSolidBrush(0x000000);
		FillRect(hMemDC2,&rt,BGBrush);
		DeleteObject(BGBrush);
	}


	int mm=0;
		for(int i=0;i<crt.right;i++)
			for(int j=0;j<crt.bottom;j++)
				if(mm<cnt[j][i])
					mm=cnt[j][i];

		for(int i=0;i<crt.right;i++)
			for(int j=0;j<crt.bottom;j++){	
//				if(test[j][i]!=0)
//					SetPixel(hMemDC2,i*Width/crt.right,j*Height/crt.bottom,RGB(255*cnt[j][i]/mm,255,255*cnt[j][i]/mm));
			}

		hPen =  CreatePen(PS_SOLID, 1, RGB(250,0,0));
		OldPen = (HPEN)SelectObject(hMemDC2,hPen);
		hBrush = CreateSolidBrush(RGB(250,0,0));
		OldBrush = (HBRUSH)SelectObject(hMemDC2, hBrush);

		if(unit){
			int tx,ty;

			tx = unit->GetX()*Width/crt.right;
			ty = unit->GetY()*Height/crt.bottom;
			Rectangle(hMemDC2,tx-3,ty-3,tx+3,ty+3);

			SetPixel(hMemDC2,tx,ty,RGB(255,0,0));
		}

		SelectObject(hMemDC2, OldBrush);
		SelectObject(hMemDC2, OldPen);

		SetPixel(hMemDC2,GreenX,GreenY,RGB(255,0,0));

		for(int i=0;i<top;i++)
			SetPixel(hMemDC2,stack[i][1],stack[i][0],RGB(255,0,255));

//	StretchBlt(*hdc,crt.left,crt.top,crt.right,crt.bottom,hMemDC2,0,0,Width,Height,SRCCOPY);
		BitBlt(*hdc,crt.left,crt.top,crt.right,crt.bottom,hMemDC2,0,0,SRCCOPY);
	SelectObject(BackDC, OldBitmap);
	SelectObject(hMemDC2, OldBitmap2);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(TBitmap);
	DeleteDC(hMemDC2);
	DeleteDC(BackDC);
}

void MSG_PAINT(HWND hWnd)
{
	char LStr[128];

	PAINTSTRUCT ps;
	HDC hdc;
	HBITMAP hBitmap, OldBitmap, OldBitmap2;
	HBRUSH BGBrush;
    HFONT Font, OldFont;

	hdc = BeginPaint(hWnd,&ps);
	if(Display != NULL)	DeleteDC(Display);
	Display = CreateCompatibleDC(hdc);
	if(Display_BITMAP != NULL)	DeleteObject(Display_BITMAP);
	Display_BITMAP = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
	OldBitmap2 = (HBITMAP)SelectObject(Display, Display_BITMAP);
		
		BGBrush = CreateSolidBrush(0xffff00);
		FillRect(Display, &crt, BGBrush);
		DeleteObject(BGBrush);

		Print(&Display);

	BitBlt(hdc, 0, 0, crt.right, crt.bottom, Display, 0, 0, SRCCOPY);

	EndPaint(hWnd,&ps);
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch(iMessage) {
	case WM_TIMER:		RealTime();
						InvalidateRect(hWnd, NULL, FALSE);	break;
	case WM_PAINT:		MSG_PAINT(hWnd);					break;
	case WM_KEYDOWN:
		if(wParam == VK_SPACE){
			Make_Unit(unit);
			ASTAR_SW = 1;
		}
		break;
	case WM_CREATE:	
						SetTimer(hWnd,0,10,NULL);			break;
	case WM_DESTROY:	PostQuitMessage(0);					break;
	}
	return DefWindowProc( hWnd, iMessage, wParam, lParam );
}

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow )
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = CreateSolidBrush(RGB( 255,255,255 ) );
	WndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	WndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = ( WNDPROC )WndProc;
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = TEXT( "ASTAR" );
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass( &WndClass );

	Init_API();
	int width, height;
	width = min(Width+16,1280);
	height = min(Height+38, 760);

	hWnd = CreateWindow( TEXT( "ASTAR" ), TEXT( "ASTAR" ),
	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 0, 0,
	width, height, NULL, (HMENU)NULL, hInstance, NULL );

	GetClientRect(hWnd, &crt);
//	Make_Unit(unit);

	//1280, 760

	ShowWindow( hWnd, nCmdShow );
 
	while( GetMessage( &Message, NULL, 0, 0 ) ) {
		TranslateMessage( &Message );
		DispatchMessage( &Message );
	}

	cvReleaseImage(&m_pImage);
	cvReleaseCapture(&m_pCapture);
	return (int)Message.wParam;
}