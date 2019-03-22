#include <opencv2\opencv.hpp>
#include <stdio.h>

//MouseFunction에서 param 구분에 사용
#define HamburgerSelect 1
#define PizzaSelect 2
#define SourceSelect 3
#define SandwichSelect 4

#define HambugerIndex 11
#define PizzaIndex 9
#define SourceIndex 7
#define SandwichIndex 11

//Main에서 띄어주는 Food 사진
//후에 Release하기 위해 전역변수로 선언
IplImage *Main;

//Main 이미지의 Max Y길이 저장
int MaxY = 0;
int MainWidth[3] = {0};

//화면에 보여지는 베이스 음식 사진
IplImage *BaseImg;

//창에 보여지는 실질적인 이미지
IplImage *FoodDesign;

//화면에 보여지는 재료 음식 사진
IplImage *ShowMaterial[14];
IplImage *ShowSource[14];
//음식 재료사진의 원본 크기
IplImage *MaterialImg[14];

//음식을 추가시 추가된 상태를 저장하기 위한 이미지
IplImage *TempImg;

//최종 음식 사진의 선따기
IplImage *EdgeImg;

//마지막에 추가된 상황을 애니메이션으로 보여주기 위한 이미지 배열
//음식이 추가될때마다 저장
IplImage *ResultImg[50];
//음식이 사진이 ResultImg에 추가될때 마다 1씩 증가
int ResultCnt = 0;

//누적 길이 저장
int AddHeight[15] = {0};
int TempAddHeight[12] = {0};

//길이 저장
int Height[15] = {0};
int Width[15] = {0};

//음식 사진의 최대 가로 길이
int MaxX = 0;
int SecondMaxX = 0;

//음식 가격 저장
int price = 0;

//선택된 재료의 RGB값을 임시적으로 저장
CvScalar ClickColor[1000][1000];

//어떤 재료가 선택되었는지 저장 (0 은 베이스 사진)
int MaterialFlag = 0;

//소스 출력시 필요
int MaxHeight[7] = {0};

// 선택한 음식 저장
int SelectFood = 0;

CvFont font;

void SaveResultImg(IplImage *src){
	ResultImg[ResultCnt] = cvCreateImage(cvSize(Width[0], FoodDesign->height),8, 3);
	for(int x = 0; x < Width[0]; x++)
		for(int y = 0; y < FoodDesign->height; y++){
			CvScalar c = cvGet2D(src, y, x);
			cvSet2D(ResultImg[ResultCnt], y, x, c);
		}
	ResultCnt++;
}

void AddPrice(int flag){
	if(SelectFood == HamburgerSelect){
		switch(flag){
		case 1:
			price += 500;
			break;
		case 2:
			price += 1000;
			break;
		case 3:
			price += 1000;
			break;
		case 4:
			price += 500;
			break;
		case 5:
			price += 500;
			break;
		case 6:
			price += 500;
			break;
		case 7:
			price += 500;
			break;
		case 8:
			price += 500;
			break;
		case 9:
			price += 500;
			break;
		case 10:
			price += 500;
			break;
		case 11:
			price += 1000;
			break;
		}
	}
	else if(SelectFood == PizzaSelect){
		switch(flag){
		case 1:
			price += 1000;
			break;
		case 2:
			price += 2000;
			break;
		case 3:
			price += 1000;
			break;
		case 4:
			price += 500;
			break;
		case 5:
			price += 1000;
			break;
		case 6:
			price += 500;
			break;
		case 7:
			price += 500;
			break;
		case 8:
			price += 1000;
			break;
		case 9:
			price += 1000;
			break;
		}
	}
	else if(SelectFood == SandwichSelect){
		switch(flag){
		case 1:
			price += 1000;
			break;
		case 2:
			price += 500;
			break;
		case 3:
			price += 1000;
			break;
		case 4:
			price += 1000;
			break;
		case 5:
			price += 500;
			break;
		case 6:
			price += 500;
			break;
		case 7:
			price += 500;
			break;
		case 8:
			price += 500;
			break;
		case 9:
			price += 500;
			break;
		case 10:
			price += 500;
			break;
		case 11:
			price += 500;
			break;
		}
	}
}

void AddText(IplImage *temp){
	char Text[20] = "Price : ";
	char str[10];
	ultoa(price, str, 10);
	strcat(Text, str);

	int ImgW = temp->width, ImgH = temp->height;

	cvPutText(temp, Text, cvPoint(5, 50), &font, CV_RGB(0, 0, 0));
}

void FoodMouseFunction(int event, int mx, int my, int flags, void *param){
	if(event == CV_EVENT_LBUTTONDOWN){
		if(mx < Width[0]){
			if(MaterialFlag){
				for(int x = 0; x < MaterialImg[MaterialFlag - 1]->width; x++){
					for(int y = 0; y < MaterialImg[MaterialFlag - 1]->height; y++){
						int tx = x + mx - (MaterialImg[MaterialFlag - 1]->width / 2);
						int ty = y + my - (MaterialImg[MaterialFlag - 1]->height / 2);
						if(tx >= 0 && tx < Width[0] && ty >=0 && ty < FoodDesign->height){
							if(ClickColor[x][y].val[0] != 255)
								cvSet2D(TempImg, ty, tx, ClickColor[x][y]);
						}
					}
				}
				AddPrice(MaterialFlag);
				MaterialFlag = 0;
				SaveResultImg(TempImg);
				IplImage * WordTempImg = cvCreateImage(cvSize(TempImg->width, TempImg->height), 8, 3);
				cvCopy(TempImg, WordTempImg);
				AddText(WordTempImg);
				if((int)param == SourceIndex) cvShowImage("SandwichSource", WordTempImg);
				else cvShowImage("FoodDesigner", WordTempImg);
				cvReleaseImage(&WordTempImg);
			}
		}
		else if(mx < Width[0] + MaxX + 1){
			for(int i = 1; i < (int)param + 1; i++){
				if(i == 1 && my < AddHeight[i]){
					MaterialFlag = i;
				}
				else if(my < AddHeight[i] && my >= AddHeight[i - 1]){
					MaterialFlag = i;
				}
			}
			if(MaterialFlag)
				for(int x = 0; x < MaterialImg[MaterialFlag - 1]->width; x++){
					for(int y = 0; y < MaterialImg[MaterialFlag - 1]->height; y++){
						ClickColor[x][y] = cvGet2D(MaterialImg[MaterialFlag - 1], y, x);
					}
				}
		}
		else{
			for(int i = 1; i < (int)param + 1; i++){
				if(i == 1 && my < AddHeight[i]){
					MaterialFlag = i + (int)param;
				}
				else if(my < AddHeight[i] && my >= AddHeight[i - 1]){
					MaterialFlag = i + (int)param;
				}
			}
			if(MaterialFlag)
				for(int x = 0; x < MaterialImg[MaterialFlag - 1]->width; x++){
					for(int y = 0; y < MaterialImg[MaterialFlag - 1]->height; y++){
						ClickColor[x][y] = cvGet2D(MaterialImg[MaterialFlag - 1], y, x);
					}
				}
		}
	}

	if(event == CV_EVENT_MOUSEMOVE){
		if(mx < Width[0] && MaterialFlag){
			cvCopy(TempImg, FoodDesign);
			for(int x = 0; x < MaterialImg[MaterialFlag - 1]->width; x++){
				for(int y = 0; y < MaterialImg[MaterialFlag - 1]->height; y++){
					int tx = x + mx - (MaterialImg[MaterialFlag - 1]->width / 2);
					int ty = y + my - (MaterialImg[MaterialFlag - 1]->height / 2);
					if(tx >= 0 && tx < Width[0] && ty >=0 && ty < FoodDesign->height){
						if(ClickColor[x][y].val[0] != 255)
							cvSet2D(FoodDesign, ty, tx, ClickColor[x][y]);
					}
				}
			}
			IplImage *WordTempImg = cvCreateImage(cvSize(FoodDesign->width, FoodDesign->height), 8, 3);
			cvCopy(FoodDesign, WordTempImg);
			AddText(WordTempImg);
			if((int)param == SourceIndex) cvShowImage("SandwichSource", WordTempImg);
			else cvShowImage("FoodDesigner", WordTempImg);
			cvReleaseImage(&WordTempImg);
		}
	}
}

void FoodDesigner(int FoodIndex){
	//화면에 보여지는 재료들의 크기 저장
	for(int Index = 0; Index < FoodIndex; Index++){
		Height[Index + 1] = ShowMaterial[Index]->height;
		Width[Index + 1] = ShowMaterial[Index]->width;
		// + 1은 구분선을 그리기 위한 공간
		AddHeight[Index + 1] += AddHeight[Index] + ShowMaterial[Index]->height + 1;
		if(MaxX < ShowMaterial[Index]->width)
			MaxX = ShowMaterial[Index]->width;
	}
	//화면에 보여지는 베이스 음식 사진의 가로, 세로 길이 저장
	Height[0] = BaseImg->height;
	Width[0] = BaseImg->width;

	//창의 세로 크기 결정
	int ResultY = 0;
	if(AddHeight[FoodIndex] > Height[0] + FoodIndex - 1)
		ResultY = AddHeight[FoodIndex];
	else
		ResultY = Height[0] + FoodIndex - 1;
	//창의 크기
	FoodDesign = cvCreateImage(cvSize(Width[0] + MaxX + 1, ResultY), 8, 3);
	TempImg = cvCreateImage(cvSize(Width[0] + MaxX + 1, ResultY), 8, 3);
	cvSet(FoodDesign, CV_RGB(255, 255, 255));

	//창에 베이스 사진 띄우기
	int StartY = FoodDesign->height - Height[0];
	for(int x = 0; x < Width[0]; x++){
		for(int y = 0; y <  Height[0]; y++){
			CvScalar c = cvGet2D(BaseImg, y, x);
			cvSet2D(FoodDesign, StartY + y, x, c);
		}
	}

	//구분선 그리기
	for(int y = 0; y < FoodDesign->height; y++){
		cvSet2D(FoodDesign, y, Width[0], CV_RGB(0,0,0));
	}

	//음식 재료 창에 띄우기
	for(int Index = 0; Index < FoodIndex; Index++){
		int StartX = Width[0] + 1;
		StartX += (MaxX / 2) - (Width[Index + 1] / 2);
		for(int x = 0; x < Width[Index + 1]; x++){
			for(int y = 0; y <  Height[Index + 1]; y++){
				CvScalar c = cvGet2D(ShowMaterial[Index], y, x);
				cvSet2D(FoodDesign, AddHeight[Index] + 1 + y, StartX + x, c);
			}
		}
		//음식 구분선 그리기
		for(int x = 0; x < MaxX && Index != FoodIndex - 1; x++){
			cvSet2D(FoodDesign,  AddHeight[Index + 1], Width[0] + 1 + x, CV_RGB(0,0,0));
		}
	}
	
	cvCopy(FoodDesign, TempImg);
	SaveResultImg(FoodDesign);

	//화면에 띄어준 음식 재료 사진 release
	cvReleaseImage(ShowMaterial);

	cvNamedWindow("FoodDesigner");
	cvMoveWindow("FoodDesigner", 20,20);
	IplImage * WordTempImg = cvCreateImage(cvSize(FoodDesign->width, FoodDesign->height), 8, 3);
	cvCopy(FoodDesign, WordTempImg);
	AddText(WordTempImg);
	cvShowImage("FoodDesigner", WordTempImg);
	cvReleaseImage(&WordTempImg);

	cvSetMouseCallback("FoodDesigner", FoodMouseFunction, (int *)FoodIndex);

}

void LoadImage(int param){
	cvDestroyWindow("FoodMain");
	cvReleaseImage(&Main);
	if(param == HamburgerSelect){
		BaseImg = cvLoadImage("Hamburger\\SDownbread.png");

		ShowMaterial[0] = cvLoadImage("Hamburger\\SSSSUpbread.png");
		MaterialImg[0] = cvLoadImage("Hamburger\\SUpbread.png");
		ShowMaterial[1] = cvLoadImage("Hamburger\\SSSSMeat.png");
		MaterialImg[1] = cvLoadImage("Hamburger\\SMeat.png");
		ShowMaterial[2] = cvLoadImage("Hamburger\\SSSSChicken.png");
		MaterialImg[2] = cvLoadImage("Hamburger\\SChicken.png");
		ShowMaterial[3] = cvLoadImage("Hamburger\\SSSSTomato.png");
		MaterialImg[3] = cvLoadImage("Hamburger\\STomato.png");
		ShowMaterial[4] = cvLoadImage("Hamburger\\SSSSEgg.png");
		MaterialImg[4] = cvLoadImage("Hamburger\\SEgg.png");
		ShowMaterial[5] = cvLoadImage("Hamburger\\SSSSCheese.png");
		MaterialImg[5] = cvLoadImage("Hamburger\\SCheese.png");
		ShowMaterial[6] = cvLoadImage("Hamburger\\SSSSYang.png");
		MaterialImg[6] = cvLoadImage("Hamburger\\SYang.png");
		ShowMaterial[7] = cvLoadImage("Hamburger\\SSSSOnion.png");
		MaterialImg[7] = cvLoadImage("Hamburger\\SOnion.png");
		ShowMaterial[8] = cvLoadImage("Hamburger\\SSSSMayo.png");
		MaterialImg[8] = cvLoadImage("Hamburger\\SMayo.png");
		ShowMaterial[9] = cvLoadImage("Hamburger\\SSSSPickle.png");
		MaterialImg[9] = cvLoadImage("Hamburger\\SPickle.png");
		ShowMaterial[10] = cvLoadImage("Hamburger\\SSSSShrimp.png");
		MaterialImg[10] = cvLoadImage("Hamburger\\SShrimp.png");
	}
	else if(param == PizzaSelect){
		BaseImg = cvLoadImage("Pizza\\SSDough.png");

		ShowMaterial[0] = cvLoadImage("Pizza\\SSSMeat.png");
		MaterialImg[0] = cvLoadImage("Pizza\\SSMeat.png");
		ShowMaterial[1] = cvLoadImage("Pizza\\SSSSSSource.png");
		MaterialImg[1] = cvLoadImage("Pizza\\SSSSource.png");
		ShowMaterial[2] = cvLoadImage("Pizza\\SSSCheese.png");
		MaterialImg[2] = cvLoadImage("Pizza\\SSCheese.png");
		ShowMaterial[3] = cvLoadImage("Pizza\\SSSPaprika.png");
		MaterialImg[3] = cvLoadImage("Pizza\\SSPaprika.png");
		ShowMaterial[4] = cvLoadImage("Pizza\\SSSShrimp.png");
		MaterialImg[4] = cvLoadImage("Pizza\\SSShrimp.png");
		ShowMaterial[5] = cvLoadImage("Pizza\\SSSOnion.png");
		MaterialImg[5] = cvLoadImage("Pizza\\SSOnion.png");
		ShowMaterial[6] = cvLoadImage("Pizza\\SSSOlive.png");
		MaterialImg[6] = cvLoadImage("Pizza\\SSOlive.png");
		ShowMaterial[7] = cvLoadImage("Pizza\\SSSPotato.png");
		MaterialImg[7] = cvLoadImage("Pizza\\SSPotato.png");
		ShowMaterial[8] = cvLoadImage("Pizza\\SSSBacon.png");
		MaterialImg[8] = cvLoadImage("Pizza\\SSBacon.png");
	}
	else if(param == SourceSelect){
		BaseImg = cvLoadImage("Sandwich\\SSBread.png");

		ShowSource[0] = cvLoadImage("Sandwich\\Source\\SSBBQ.png");
		MaterialImg[0] = cvLoadImage("Sandwich\\Source\\SSBBQ.png");
		ShowSource[1] = cvLoadImage("Sandwich\\Source\\SSDijong.png");
		MaterialImg[1] = cvLoadImage("Sandwich\\Source\\SSDijong.png");
		ShowSource[2] = cvLoadImage("Sandwich\\Source\\SSHoney.png");
		MaterialImg[2] = cvLoadImage("Sandwich\\Source\\SSHoney.png");
		ShowSource[3] = cvLoadImage("Sandwich\\Source\\SSHotchily.png");
		MaterialImg[3] = cvLoadImage("Sandwich\\Source\\SSHotchily.png");
		ShowSource[4] = cvLoadImage("Sandwich\\Source\\SSItaly.png");
		MaterialImg[4] = cvLoadImage("Sandwich\\Source\\SSItaly.png");
		ShowSource[5] = cvLoadImage("Sandwich\\Source\\SSMarinara.png");
		MaterialImg[5] = cvLoadImage("Sandwich\\Source\\SSMarinara.png");
		ShowSource[6] = cvLoadImage("Sandwich\\Source\\SSMayo.png");
		MaterialImg[6] = cvLoadImage("Sandwich\\Source\\SSMayo.png");
		ShowSource[7] = cvLoadImage("Sandwich\\Source\\SSMurs.png");
		MaterialImg[7] = cvLoadImage("Sandwich\\Source\\SSMurs.png");
		ShowSource[8] = cvLoadImage("Sandwich\\Source\\SSRench.png");
		MaterialImg[8] = cvLoadImage("Sandwich\\Source\\SSRench.png");
		ShowSource[9] = cvLoadImage("Sandwich\\Source\\SSSweetchily.png");
		MaterialImg[9] = cvLoadImage("Sandwich\\Source\\SSSweetchily.png");
		ShowSource[10] = cvLoadImage("Sandwich\\Source\\SSSweetonion.png");
		MaterialImg[10] = cvLoadImage("Sandwich\\Source\\SSSweetonion.png");
		ShowSource[11] = cvLoadImage("Sandwich\\Source\\SSSousen.png");
		MaterialImg[11] = cvLoadImage("Sandwich\\Source\\SSSousen.png");
		ShowSource[12] = cvLoadImage("Sandwich\\Source\\SSSouth.png");
		MaterialImg[12] = cvLoadImage("Sandwich\\Source\\SSSouth.png");
		ShowSource[13] = cvLoadImage("Sandwich\\Source\\SSWine.png");
		MaterialImg[13] = cvLoadImage("Sandwich\\Source\\SSWine.png");
		ShowMaterial[0] = cvLoadImage("Sandwich\\Material\\SSSHam.png");
		ShowMaterial[1] = cvLoadImage("Sandwich\\Material\\SSSPickle.png");
		ShowMaterial[2] = cvLoadImage("Sandwich\\Material\\SSSCheese.png");
		ShowMaterial[3] = cvLoadImage("Sandwich\\Material\\SSSAbocado.png");
		ShowMaterial[4] = cvLoadImage("Sandwich\\Material\\SSSOi.png");
		ShowMaterial[5] = cvLoadImage("Sandwich\\Material\\SSSOnion.png");
		ShowMaterial[6] = cvLoadImage("Sandwich\\Material\\SSSOlive.png");
		ShowMaterial[7] = cvLoadImage("Sandwich\\Material\\SSSTomato.png");
		ShowMaterial[8] = cvLoadImage("Sandwich\\Material\\SSSYang.png");
		ShowMaterial[9] = cvLoadImage("Sandwich\\Material\\SSSHala.png");
		ShowMaterial[10] = cvLoadImage("Sandwich\\Material\\SSSPimang.png");
	}
	else if(param == SandwichSelect){
		MaterialImg[0] = cvLoadImage("Sandwich\\Material\\SSHam.png");
		MaterialImg[1] = cvLoadImage("Sandwich\\Material\\SSPickle.png");
		MaterialImg[2] = cvLoadImage("Sandwich\\Material\\SSCheese.png");
		MaterialImg[3] = cvLoadImage("Sandwich\\Material\\SSAbocado.png");
		MaterialImg[4] = cvLoadImage("Sandwich\\Material\\SSOi.png");
		MaterialImg[5] = cvLoadImage("Sandwich\\Material\\SSOnion.png");
		MaterialImg[6] = cvLoadImage("Sandwich\\Material\\SSOlive.png");
		MaterialImg[7] = cvLoadImage("Sandwich\\Material\\SSTomato.png");
		MaterialImg[8] = cvLoadImage("Sandwich\\Material\\SSYang.png");
		MaterialImg[9] = cvLoadImage("Sandwich\\Material\\SSHala.png");
		MaterialImg[10] = cvLoadImage("Sandwich\\Material\\SSPimang.png");
	}
}

void SandwichSource(int FoodIndex){
	//화면에 보여지는 재료들의 크기 저장
	for(int Index = 0; Index < FoodIndex; Index++){
		Height[Index + 1] = ShowSource[Index]->height;
		Width[Index + 1] = ShowSource[Index]->width;
		Height[Index + 1 + FoodIndex] = ShowSource[Index + FoodIndex]->height;
		Width[Index + 1 + FoodIndex] = ShowSource[Index + FoodIndex]->width;
		if(Height[Index + 1] > ShowSource[Index + FoodIndex]->height){
			MaxHeight[Index + 1] = Height[Index + 1];
			AddHeight[Index + 1] += AddHeight[Index] + Height[Index + 1] + 1;
		}else{
			MaxHeight[Index + 1] = ShowSource[Index + FoodIndex]->height;
			AddHeight[Index + 1] += AddHeight[Index] + ShowSource[Index + FoodIndex]->height + 1;
		}
		if(SecondMaxX < ShowSource[Index + FoodIndex]->width)
			SecondMaxX = ShowSource[Index + FoodIndex]->width;
		else if(MaxX < ShowSource[Index]->width)
			MaxX = ShowSource[Index]->width;
	}
	
	for(int Index = 0; Index < SandwichIndex; Index++){
		TempAddHeight[Index + 1] += TempAddHeight[Index] + ShowMaterial[Index]->height + 1;
	}

	//화면에 보여지는 베이스 음식 사진의 가로, 세로 길이 저장
	Height[0] = BaseImg->height;
	Width[0] = BaseImg->width;
	//창의 크기
	FoodDesign = cvCreateImage(cvSize(Width[0] + MaxX + SecondMaxX + 2, TempAddHeight[11]), 8, 3);
	TempImg = cvCreateImage(cvSize(Width[0] + MaxX + SecondMaxX + 2, TempAddHeight[11]), 8, 3);
	cvSet(FoodDesign, CV_RGB(255, 255, 255));

	//창에 베이스 사진 띄우기
	int StartY = FoodDesign->height - Height[0];
	for(int x = 0; x < Width[0]; x++){
		for(int y = 0; y <  Height[0]; y++){
			CvScalar c = cvGet2D(BaseImg, y, x);
			cvSet2D(FoodDesign, StartY + y, x, c);
		}
	}

	//구분선 그리기
	for(int y = 0; y < FoodDesign->height; y++){
		cvSet2D(FoodDesign, y, Width[0], CV_RGB(0,0,0));
	}
	
	//음식 재료 창에 띄우기
	for(int Index = 0; Index < FoodIndex; Index++){
		int StartX = Width[0] + 1;
		StartX += (MaxX / 2) - (Width[Index + 1] / 2);
		int StartY = (MaxHeight[Index + 1] / 2) - (Height[Index + 1] / 2);
		for(int x = 0; x < Width[Index + 1]; x++){
			for(int y = 0; y <  Height[Index + 1]; y++){
				CvScalar c = cvGet2D(ShowSource[Index], y, x);
				if(Index != 0) cvSet2D(FoodDesign, AddHeight[Index] + 1 + StartY + y, StartX + x, c);
				else cvSet2D(FoodDesign, 1 + StartY + y, StartX + x, c);
			}
		}
		//음식 구분선 그리기
		for(int x = 0; x < MaxX; x++){
			cvSet2D(FoodDesign,  AddHeight[Index + 1], Width[0] + 1 + x, CV_RGB(0,0,0));
		}
	}
	for(int y = 0; y < FoodDesign->height; y++){
		cvSet2D(FoodDesign,  y, Width[0] + 1 + MaxX, CV_RGB(0,0,0));
	}
	for(int Index = FoodIndex ; Index < FoodIndex * 2; Index++){
		int StartX = Width[0] + MaxX + 2;
		int StartY = (MaxHeight[Index - FoodIndex + 1] / 2) - (Height[Index + 1] / 2);
		StartX += (SecondMaxX / 2) - (Width[Index + 1] / 2);
		for(int x = 0; x < Width[Index + 1]; x++){
			for(int y = 0; y <  Height[Index + 1]; y++){
				CvScalar c = cvGet2D(ShowSource[Index], y, x);
				if(Index != FoodIndex) cvSet2D(FoodDesign, AddHeight[Index - FoodIndex] + StartY + 1 + y, StartX + x, c);
				else cvSet2D(FoodDesign, StartY + 1 + y, StartX + x, c);
			}
		}
		//음식 구분선 그리기
		for(int x = 0; x < SecondMaxX; x++){
			cvSet2D(FoodDesign,  AddHeight[Index - FoodIndex + 1], Width[0] + MaxX + 2 + x, CV_RGB(0,0,0));
		}
	}
	cvCopy(FoodDesign, TempImg);
	SaveResultImg(FoodDesign);

	//화면에 띄어준 음식 재료 사진 release
	cvReleaseImage(ShowSource);

	cvNamedWindow("SandwichSource");
	cvMoveWindow("SandwichSource", 20,20);

	IplImage * WordTempImg = cvCreateImage(cvSize(FoodDesign->width, FoodDesign->height), 8, 3);
	cvCopy(FoodDesign, WordTempImg);
	AddText(WordTempImg);
	cvShowImage("SandwichSource", WordTempImg);

	cvSetMouseCallback("SandwichSource", FoodMouseFunction, (int *)FoodIndex);

	while(' ' != cvWaitKey(0));
	StartY = FoodDesign->height - Height[0];
	for(int x = 0; x < Width[0]; x++){
		for(int y = 0; y <  Height[0]; y++){
			CvScalar c = cvGet2D(FoodDesign, StartY + y, x);
			cvSet2D(BaseImg, y, x, c);
		}
	}
	for(int i = 0; i < 15; i++)
		AddHeight[i] = 0;
	MaxX = 0;
	cvDestroyWindow("SandwichSource");
	SelectFood = SandwichSelect;
	LoadImage(SandwichSelect);
	FoodDesigner(SandwichIndex);
}

void MainMouseFunction(int event, int x, int y, int flags, void *param){
	if(event == CV_EVENT_LBUTTONDOWN){
		if(x < MainWidth[0]){
			SelectFood = HamburgerSelect;
			price += 500;
			LoadImage(HamburgerSelect);
			FoodDesigner(HambugerIndex);
		}
		else if(x < MainWidth[1] + MainWidth[0] + 1){
			SelectFood = PizzaSelect;
			price += 10000;
			LoadImage(PizzaSelect);
			FoodDesigner(PizzaIndex);
		}
		else{
			price += 2000;
			LoadImage(SourceSelect);
			SandwichSource(SourceIndex);
		}
	}
}

void interporation(IplImage *img, IplImage *img2){
	IplImage *interImg = cvCreateImage(cvSize(img->width, img->height), 8, 3);
	
	float alpha = 0.0f;
	float small = 0.125f;
	float delta = small;
	float min_alpha = 0.0f;
	float max_alpha = 1.0f;

	int key = -1;
	while(key < 0){
		for(int i = 0; i < img->width; i++)
			for(int j = 0; j < img->height; j++){
				CvScalar s1 = cvGet2D(img, j, i);
				CvScalar s2 = cvGet2D(img2, j, i);

				CvScalar s;
				s.val[0] = (1.0 - alpha) * s1.val[0] + alpha * s2.val[0];
				s.val[1] = (1.0 - alpha) * s1.val[1] + alpha * s2.val[1];
				s.val[2] = (1.0 - alpha) * s1.val[2] + alpha * s2.val[2];
				cvSet2D(interImg, j, i, s);
			}
		key = cvWaitKey(10);
		IplImage * WordTempImg = cvCreateImage(cvSize(interImg->width, interImg->height), 8, 3);
		cvCopy(interImg, WordTempImg);
		AddText(WordTempImg);
		cvShowImage("ResultFoodAnimation", WordTempImg);
		cvReleaseImage(&WordTempImg);
		if(alpha > max_alpha) break;
		alpha += delta;
	}
	cvReleaseImage(&interImg);
}

void Edge(IplImage *img){
	int EdgeSize = 10;
	for(int x = 0; x < img->width; x++){
		for(int y = 0; y < img->height; y++){
			CvScalar c = cvGet2D(img, y, x);
			CvScalar temp[4];
			if(x > EdgeSize && y > EdgeSize && x < img->width - EdgeSize - 1 && y < img->height - EdgeSize - 1){
				temp[0] = cvGet2D(img, y - EdgeSize, x);
				temp[1] = cvGet2D(img, y + EdgeSize, x);
				temp[2] = cvGet2D(img, y, x - EdgeSize);
				temp[3] = cvGet2D(img, y, x + EdgeSize);
			}
			else{
				temp[0] = CV_RGB(255, 255, 255);
				temp[1] = CV_RGB(255, 255, 255);
				temp[2] = CV_RGB(255, 255, 255);
				temp[3] = CV_RGB(255, 255, 255);
			}
			if(c.val[0] + temp[0].val[0] < 510 && (c.val[0] == 255 || temp[0].val[0] == 255))
				cvSet2D(EdgeImg, y, x, CV_RGB(0, 0, 0));
			else if(c.val[0] + temp[1].val[0] < 510 && (c.val[0] == 255 || temp[1].val[0] == 255))
				cvSet2D(EdgeImg, y, x, CV_RGB(0, 0, 0));
			else if(c.val[0] + temp[2].val[0] < 510 && (c.val[0] == 255 || temp[2].val[0] == 255))
				cvSet2D(EdgeImg, y, x, CV_RGB(0, 0, 0));
			else if(c.val[0] + temp[3].val[0] < 510 && (c.val[0] == 255 || temp[3].val[0] == 255))
				cvSet2D(EdgeImg, y, x, CV_RGB(0, 0, 0));
			else 
				cvSet2D(EdgeImg, y, x, CV_RGB(255, 255, 255));
		}
	}
	/*
	for(int x = EdgeSize; x < img->width - EdgeSize - 1; x++){
		for(int y = EdgeSize; y < img->height - EdgeSize - 1; y++){
			CvScalar c = cvGet2D(img, y, x);
			CvScalar temp[4] = {cvGet2D(img, y - EdgeSize, x), cvGet2D(img, y + EdgeSize, x), cvGet2D(img, y, x - EdgeSize), cvGet2D(img, y, x + EdgeSize)};
			if(c.val[0] + temp[0].val[0] < 510 && (c.val[0] == 255 || temp[0].val[0] == 255))
				cvSet2D(EdgeImg, y, x, CV_RGB(0, 0, 0));
			else if(c.val[0] + temp[1].val[0] < 510 && (c.val[0] == 255 || temp[1].val[0] == 255))
				cvSet2D(EdgeImg, y, x, CV_RGB(0, 0, 0));
			else if(c.val[0] + temp[2].val[0] < 510 && (c.val[0] == 255 || temp[2].val[0] == 255))
				cvSet2D(EdgeImg, y, x, CV_RGB(0, 0, 0));
			else if(c.val[0] + temp[3].val[0] < 510 && (c.val[0] == 255 || temp[3].val[0] == 255))
				cvSet2D(EdgeImg, y, x, CV_RGB(0, 0, 0));
			else 
				cvSet2D(EdgeImg, y, x, CV_RGB(255, 255, 255));
		}
	}
	*/
}

void NoiseFiltering(IplImage *img){
	double kernel[3][3];
	kernel[0][0] = 1.0 / 9.0; kernel[0][1] = 1.0 / 9.0; kernel[0][2] = 1.0 / 9.0;
	kernel[1][0] = 1.0 / 9.0; kernel[1][1] = 1.0 / 9.0; kernel[1][2] = 1.0 / 9.0; 
	kernel[2][0] = 1.0 / 9.0; kernel[2][1] = 1.0 / 9.0; kernel[2][2] = 1.0 / 9.0;
	for(int x = 1; x < img -> width - 1 ; x++){
		for(int y = 1; y < img -> height - 1; y++){
			CvScalar finalColor;
			finalColor.val[0] = finalColor.val[1] = finalColor.val[2] = 0.0;
			for(int u = 0; u < 3; u++){
				for(int v = 0; v < 3; v++){
					CvScalar color = cvGet2D(img, y + v - 1, x + u - 1);
					finalColor.val[0] += kernel[u][v] * color.val[0];
					finalColor.val[1] += kernel[u][v] * color.val[1];
					finalColor.val[2] += kernel[u][v] * color.val[2];
				}
			}
			cvSet2D(img, y, x, finalColor);
		}
	}
}

void AddImage(IplImage *bac, IplImage *food){
	int StartX = (bac->width / 2) - (food->width / 2);
	int StartY = (bac->height / 2) - (food->height / 2);
	for(int x = 0; x < food->width; x++){
		for(int y = 0; y < food->height; y++){
			CvScalar c = cvGet2D(food, y, x);
			if(c.val[0] != 255)
				cvSet2D(bac, y + StartY, x + StartX, c);
		}
	}
}

void AddNoise(IplImage *img){
	int StartX = (img->width / 2) - (EdgeImg->width / 2);
	int StartY = (img->height / 2) - (EdgeImg->height / 2);
	for(int x = 0; x < EdgeImg->width; x++){
		for(int y = 0; y < EdgeImg->height; y++){
			CvScalar temp = cvGet2D(EdgeImg, y, x);
			if(temp.val[0] == 0){
				CvScalar c = cvGet2D(img, StartY + y, StartX + x);
				cvSet2D(EdgeImg, y, x, c);
			}
		}
	}
}

void FontInit(){
	// 글꼴 및 크기 초기화
	// cvInitFont(CvFont *font, int face, int hscale, int vscale);
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 3, 3);
}

void Bling(IplImage *img){
	IplImage *ResultImage = cvCreateImage(cvSize(img->width, img->height), 8, 3);
	//IplImage *star = cvLoadImage("SStwinkle.png");
	IplImage *star = cvLoadImage("star.png");
	int step = 10;
	//cvShowImage("star",star);
	for(int Scale = 1; Scale < step * 2; Scale++){
		cvCopy(img, ResultImage);
		float temp;
		if(Scale > step){
			temp = 2 * step - Scale;
		}
		else
			temp = Scale;
		IplImage *Simg = cvCreateImage(cvSize(star->width * (float)(temp / (float)step), star->height * (float)(temp / (float)step)) ,8 ,3);
		//printf("Convert %f, %f\n", star->width * (float)(temp / (float)step), star->height * (float)(temp / (float)step));
		cvResize(star, Simg, CV_INTER_AREA);
		int StartX = 140 - Simg->width / 2;
		int StartY = 160 - Simg->height / 2;
		for(int x = 0; x < Simg->width; x++){
			for(int y = 0; y < Simg->height; y++){
				CvScalar c = cvGet2D(Simg, y, x);
				if(c.val[0] != 255){
					cvSet2D(ResultImage, y + StartY, x + StartX, CV_RGB(255, 255, 255));
				}
			}
		}
		cvReleaseImage(&Simg);
		cvWaitKey(1);
		cvShowImage("ResultFoodAnimation", ResultImage);
	}
	for(int Scale = 1; Scale < step * 2; Scale++){
		cvCopy(img, ResultImage);
		float temp;
		if(Scale > step){
			temp = 2 * step - Scale;
		}
		else
			temp = Scale;
		IplImage *Simg = cvCreateImage(cvSize(star->width * (float)(temp / (float)step), star->height * (float)(temp / (float)step)) ,8 ,3);
		//printf("Convert %f, %f\n", star->width * (float)(temp / (float)step), star->height * (float)(temp / (float)step));
		cvResize(star, Simg, CV_INTER_AREA);
		int StartX = 560 - Simg->width / 2;
		int StartY = 330 - Simg->height / 2;
		for(int x = 0; x < Simg->width; x++){
			for(int y = 0; y < Simg->height; y++){
				CvScalar c = cvGet2D(Simg, y, x);
				if(c.val[0] != 255){
					cvSet2D(ResultImage, y + StartY, x + StartX, CV_RGB(255, 255, 255));
				}
			}
		}
		cvReleaseImage(&Simg);
		cvWaitKey(1);
		cvShowImage("ResultFoodAnimation", ResultImage);
	}
	for(int Scale = 1; Scale < step * 2; Scale++){
		cvCopy(img, ResultImage);
		float temp;
		if(Scale > step){
			temp = 2 * step - Scale;
		}
		else
			temp = Scale;
		IplImage *Simg = cvCreateImage(cvSize(star->width * (float)(temp / (float)step), star->height * (float)(temp / (float)step)) ,8 ,3);
		//printf("Convert %f, %f\n", star->width * (float)(temp / (float)step), star->height * (float)(temp / (float)step));
		cvResize(star, Simg, CV_INTER_AREA);
		int StartX = 200 - Simg->width / 2;
		int StartY = 560 - Simg->height / 2;
		for(int x = 0; x < Simg->width; x++){
			for(int y = 0; y < Simg->height; y++){
				CvScalar c = cvGet2D(Simg, y, x);
				if(c.val[0] != 255){
					cvSet2D(ResultImage, y + StartY, x + StartX, CV_RGB(255, 255, 255));
				}
			}
		}
		cvReleaseImage(&Simg);
		cvWaitKey(1);
		cvShowImage("ResultFoodAnimation", ResultImage);
	}
	cvShowImage("ResultFoodAnimation", img);
	cvReleaseImage(&star);
	cvReleaseImage(&ResultImage);
}

void main(){
	//Main에서 띄어주는 Food 완성 이미지
	FontInit();
	IplImage *MainImg[3];
	MainImg[0] = cvLoadImage("SBurger.png");
	MainImg[1] = cvLoadImage("SPizza.png");
	MainImg[2] = cvLoadImage("SSandwich.png");

	cvNamedWindow("FoodMain");
	cvMoveWindow("FoodMain", 20,20);
	
	for(int Index = 0; Index < 3; Index++){
		MainWidth[Index] = MainImg[Index]->width;
		if(MainImg[Index]->height > MaxY)
			MaxY = MainImg[Index]->height;
	}

	Main = cvCreateImage(cvSize(MainImg[0]->width + 1 + MainImg[1]->width + 1 + MainImg[2]->width, MaxY), 8, 3);
	cvSet(Main, CV_RGB(255,255,255));

	int AddX = 0;
	for(int Index = 0; Index < 3; Index++){
		int StartY = (MaxY / 2) - (MainImg[Index]->height / 2);
		for(int x = 0; x < MainImg[Index]->width; x++){
			for(int y = 0; y <  MainImg[Index]->height; y++){
				CvScalar c = cvGet2D(MainImg[Index], y, x);
				cvSet2D(Main, StartY + y, x + AddX, c);
			}
		}
		AddX += MainImg[Index]->width;
		for(int y = 0; y < MaxY && Index != 2; y++){
			cvSet2D(Main, y, AddX, CV_RGB(0, 0, 0));
		}
		AddX++;
	}
	cvReleaseImage(MainImg);

	cvShowImage("FoodMain", Main);

	cvSetMouseCallback("FoodMain", MainMouseFunction);
	
	while(' ' != cvWaitKey(0));
	cvDestroyWindow("FoodDesigner");
	cvReleaseImage(&BaseImg);
	cvReleaseImage(&FoodDesign);

	cvNamedWindow("ResultFoodAnimation"); 
	cvMoveWindow("ResultFoodAnimation", 20,20);
	for(int Index = 0; Index < ResultCnt - 1; Index++)
		interporation(ResultImg[Index], ResultImg[Index + 1]);

	ResultCnt --;
	
	EdgeImg = cvCreateImage(cvSize(ResultImg[ResultCnt]->width, ResultImg[ResultCnt]->height), 8, 3);
	cvSet(EdgeImg, CV_RGB(255,255,255));
	Edge(ResultImg[ResultCnt]);
	
	IplImage *saveImg = cvCreateImage(cvSize(ResultImg[ResultCnt]->width, ResultImg[ResultCnt]->height), 8, 3);
	cvCopy(ResultImg[ResultCnt], saveImg);

	while(true){
		char ch = cvWaitKey();
		if(ch == 'e' ||  ch == 'E'){
			cvShowImage("ResultFoodAnimation", EdgeImg);
		}
		if(ch == 's' || ch == 'S'){
			cvSaveImage("My_Food.jpg", saveImg);
		}
		if(ch == 't' || ch == 'T'){
			Bling(ResultImg[ResultCnt]);
		}
		if(ch == 'b' || ch == 'B'){
			IplImage *bac;
			switch(SelectFood){
			case HamburgerSelect :
				bac = cvLoadImage("hamburgerbackground.jpg");
				AddImage(bac, ResultImg[ResultCnt]);
				for(int i = 0; i < 15; i++)
					NoiseFiltering(bac);
				AddNoise(bac);
				AddImage(bac, ResultImg[ResultCnt]);
				AddImage(bac, EdgeImg);
				cvShowImage("ResultFoodAnimation", bac);
				break;
			case PizzaSelect :
				bac = cvLoadImage("pizzabackground.jpg");
				AddImage(bac, ResultImg[ResultCnt]);
				for(int i = 0; i < 15; i++)
					NoiseFiltering(bac);
				AddNoise(bac);
				AddImage(bac, ResultImg[ResultCnt]);
				AddImage(bac, EdgeImg);
				cvShowImage("ResultFoodAnimation", bac);
				break;
			case SandwichSelect:
				bac = cvLoadImage("Ssandwichbackground.jpg");
				AddImage(bac, ResultImg[ResultCnt]);
				for(int i = 0; i < 15; i++)
					NoiseFiltering(bac);
				AddNoise(bac);
				AddImage(bac, ResultImg[ResultCnt]);
				AddImage(bac, EdgeImg);
				cvShowImage("ResultFoodAnimation", bac);
				break;
			}
			saveImg = cvCreateImage(cvSize(bac->width, bac->height), 8, 3);
			cvCopy(bac, saveImg);
		}
		if(ch == ' ')
			break;
	}
	cvDestroyWindow("ResultFoodAnimation");
}