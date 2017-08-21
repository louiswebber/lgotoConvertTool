
// fontLibToolsAppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "fontLibToolsApp.h"
#include "fontLibToolsAppDlg.h"
#include "queueMsgDef.h"
#include "afxdialogex.h"
#include <queue>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWnd *pwnd;
HWND hwnd;
std::queue<int> threadMsgQueue;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CfontLibToolsAppDlg 对话框

DWORD WINAPI fontThreadProc(LPVOID lpParameter);

CfontLibToolsAppDlg::CfontLibToolsAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CfontLibToolsAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CfontLibToolsAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfontLibToolsAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CfontLibToolsAppDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CfontLibToolsAppDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CfontLibToolsAppDlg 消息处理程序

BOOL CfontLibToolsAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	hwnd	= m_hWnd;//获取对话框句柄
	pwnd	= FromHandle(hwnd);//根据句柄转化成对话框指针
	pSrcFile	= NULL;
	scanDirectFlag	= USER_DIRECT_R2L;//初始化扫描方向默认从右到左
	fontThreadHandle = CreateThread(NULL, 0, fontThreadProc, NULL, 0, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CfontLibToolsAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CfontLibToolsAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CfontLibToolsAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CfontLibToolsAppDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE);// TRUE for FileOpen, FALSE for FileSaveAs
	fileDlg.m_ofn.lpstrTitle=_T("选择要计算的文件");
	fileDlg.m_ofn.lpstrFilter=_T("asm文件(*.asm)\0*.asm\0所有文件(*.*)\0*.*\0");
	if( IDOK ==fileDlg.DoModal())
	{
		srcFileName = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT1, srcFileName);

		int lenth	= 0;
		char *buf;

		lenth	= WideCharToMultiByte(CP_ACP, 0, srcFileName, -1, NULL, 0, NULL, 0);
		buf	= (char*)calloc(lenth, sizeof(char));
		memset(buf, 0, lenth*(sizeof(unsigned char)));
		WideCharToMultiByte(CP_ACP, 0, srcFileName, -1, buf, lenth, NULL, 0);

		fopen_s(&pSrcFile, buf, "rb");

		free(buf);
		buf = 0;

		threadMsgQueue.push(USER_QUEUE_MSG_OPEN_FILE);
	}
}

void CfontLibToolsAppDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(FALSE);// TRUE for FileOpen, FALSE for FileSaveAs
	fileDlg.m_ofn.lpstrTitle=_T("选择要计算的文件");
	fileDlg.m_ofn.lpstrFilter=_T("asm文件(*.asm)\0*.asm\0");
	if( IDOK ==fileDlg.DoModal())
	{
		outFileName = fileDlg.GetPathName() + _T(".asm");
		SetDlgItemText(IDC_EDIT2, outFileName);

		int lenth	= 0;
		char *buf;

		lenth	= WideCharToMultiByte(CP_ACP, 0, outFileName, -1, NULL, 0, NULL, 0);
		buf	= (char*)calloc(lenth, sizeof(char));
		memset(buf, 0, lenth*(sizeof(char)));
		WideCharToMultiByte(CP_ACP, 0, outFileName, -1, buf, lenth, NULL, 0);
		//CFile file( fileDlg.GetPathName(), CFile::modeRead);

		//strcat_s(buf, (lenth + 20)*(sizeof(char)), ".asm");
		if(0 != fopen_s(&pOutFile, buf, "w+"))
		{
		}

		free(buf);
		buf = 0;

		threadMsgQueue.push(USER_QUEUE_MSG_OUTPUT_FILE);
	}
}

int fontGetSrcFileBytes(FILE* fp, unsigned char* pOutBuf, DWORD len)
{
	CListBox *debugList = (CListBox*)GetDlgItem(hwnd, IDC_LIST1);
	
	if((NULL == pOutBuf) || (NULL == fp))
	{
		//debugList->AddString(_T("point is NULL\n"));
		return -1;
	}

	fread_s(pOutBuf, len, len, 1, fp);
	return 0;
}

int fontGetConvertConfig(unsigned char *srcBuf, DWORD lenth, DWORD *column, DWORD *row)
{
	DWORD i = 0, _col, _row;
	unsigned char * pAddrSrc	= srcBuf;
	unsigned char * pAddrDst	= srcBuf;

	if((NULL == srcBuf) || (NULL == column) || (NULL == row))
	{
		return -1;
	}

	_col	= 0;
	pAddrSrc	= srcBuf;
	for(i = 0; i < lenth / USER_STRING_BYTES; i ++)
	{
		if((*pAddrSrc == 0x0D) && (*(pAddrSrc + 1) == 0x0A))
		{
			break;
		}
		else
		{
			_col ++;
		}
		pAddrSrc += USER_STRING_BYTES;
	}

	pAddrSrc	= srcBuf;
	pAddrDst	= srcBuf;

	for(i = 0; i < lenth / USER_STRING_BYTES; i ++)
	{
		if((*pAddrSrc == 0x0D) && (*(pAddrSrc + 1) == 0x0A))
		{
		}
		else
		{
			*pAddrDst	= *pAddrSrc;
			*(pAddrDst + 1)	= *(pAddrSrc + 1);
			pAddrDst += USER_STRING_BYTES;
		}
		pAddrSrc += USER_STRING_BYTES;
	}

	_row = ((DWORD)pAddrDst - (DWORD)srcBuf) / (_col * USER_STRING_BYTES);

	if(0 != (((DWORD)pAddrDst - (DWORD)srcBuf) % _col))
	{
		MessageBox(NULL,_T("请检查最后一行内容是否足够！"), _T("注意"), MB_OK);
	}

	*column	= _col;
	*row	= _row;
	return 0;
}

int fontOutputFile(FILE *fpOut, unsigned char *srcBuf, DWORD column, DWORD row, char dir)
{
	DWORD lenth	= column * row;
	DWORD i,j,k,data;
	unsigned char *pSrcAddr;
	char outBuf[50],h2StrBuf[8], *pOutBufAddr, tempData;

	if((NULL == fpOut) || (NULL == srcBuf))
	{
		return -1;
	}

	if(USER_DIRECT_R2L == dir)
	{
		for(i = column; i > 0; i --)
		{
			data	= 0;
			for(j = row; j > 0; j --)
			{
				pSrcAddr = srcBuf + (column * (j - 1) + i - 1) * USER_STRING_BYTES;
				data <<= 1;
				if((0xA1 == *pSrcAddr) && (0xF3 == *(pSrcAddr + 1)))
				{
					data |= 0x00000001;
				}
			}
			for(k = 8; k > 0; k--)
			{
				tempData = (char)((data >> ((k - 1) * 4))&0x0000000F) ;
				if((tempData >= 0) && (tempData <= 9))
				{
					h2StrBuf[8 - k]	= '0' + tempData;
				}
				else if(((tempData >= 10) && (tempData <= 15)))
				{
					h2StrBuf[8 - k]	= 'a' + tempData - 10;
				}
			}
			memset(outBuf, 0, 50);
			strcat_s(outBuf, 50, "lgoto    ");
			pOutBufAddr	= outBuf + strlen(outBuf);
			memset(pOutBufAddr, 0, 50 - strlen(outBuf));
			if(row >= 16)
			{
				*(pOutBufAddr + 0)	= h2StrBuf[4];
				*(pOutBufAddr + 1)	= h2StrBuf[5];
				*(pOutBufAddr + 2)	= h2StrBuf[6];
				*(pOutBufAddr + 3)	= h2StrBuf[7];
			}
			else if(row >= 11)
			{
				*(pOutBufAddr + 0)	= h2StrBuf[5];
				*(pOutBufAddr + 1)	= h2StrBuf[6];
				*(pOutBufAddr + 2)	= h2StrBuf[7];
			}
			else if(row >= 7)
			{
				*(pOutBufAddr + 0)	= h2StrBuf[6];
				*(pOutBufAddr + 1)	= h2StrBuf[7];
			}
			else
			{
			}
			strcat_s(outBuf, 50, "h\r\n");
			fwrite(outBuf, sizeof(char), strlen(outBuf), fpOut);
		}
	}
	else if(USER_DIRECT_L2R == dir)
	{
		for(i = 0; i < column; i ++)
		{
			data	= 0;
			for(j = 0; j < row; j ++)
			{
				pSrcAddr = srcBuf + (column * j + i) * USER_STRING_BYTES;
				data <<= 1;
				if((0xA1 == *pSrcAddr) && (0xF3 == *(pSrcAddr + 1)))
				{
					data |= 0x00000001;
				}
			}
			for(k = 8; k > 0; k--)
			{
				tempData = (char)((data >> ((k - 1) * 4))&0x0000000F) ;
				if((tempData >= 0) && (tempData <= 9))
				{
					h2StrBuf[8 - k]	= '0' + tempData;
				}
				else if(((tempData >= 10) && (tempData <= 15)))
				{
					h2StrBuf[8 - k]	= 'a' + tempData - 10;
				}
			}
			memset(outBuf, 0, 50);
			strcat_s(outBuf, 50, "lgoto    ");
			pOutBufAddr	= outBuf + strlen(outBuf);
			memset(pOutBufAddr, 0, 50 - strlen(outBuf));
			if(row >= 16)
			{
				*(pOutBufAddr + 0)	= h2StrBuf[4];
				*(pOutBufAddr + 1)	= h2StrBuf[5];
				*(pOutBufAddr + 2)	= h2StrBuf[6];
				*(pOutBufAddr + 3)	= h2StrBuf[7];
			}
			else if(row >= 11)
			{
				*(pOutBufAddr + 0)	= h2StrBuf[5];
				*(pOutBufAddr + 1)	= h2StrBuf[6];
				*(pOutBufAddr + 2)	= h2StrBuf[7];
			}
			else if(row >= 7)
			{
				*(pOutBufAddr + 0)	= h2StrBuf[6];
				*(pOutBufAddr + 1)	= h2StrBuf[7];
			}
			else
			{
			}
			strcat_s(outBuf, 50, "h\r\n");
			fwrite(outBuf, sizeof(char), strlen(outBuf), fpOut);
		}
	}
	else
	{
	}
	return 0;
}

DWORD WINAPI fontThreadProc(LPVOID lpParameter)
{
	int msg;
	CfontLibToolsAppDlg *dlg	= (CfontLibToolsAppDlg*)pwnd;//获取对话框指针
	//HWND hwndproc = GetDlgItem(hwnd,IDD_FONTLIBTOOLSAPP_DIALOG);//获取对话框句柄

	while(1)
	{
		while(!threadMsgQueue.empty())
		{
			msg = threadMsgQueue.front();
			switch(msg)
			{
			case USER_QUEUE_MSG_OPEN_FILE:
				//listbox->ResetContent();
				{
					CListBox *debugList = (CListBox*)GetDlgItem(hwnd, IDC_LIST1);;
					DWORD fileLen = 0;

					fseek(dlg->pSrcFile, 0, SEEK_END);
					fileLen	= ftell(dlg->pSrcFile);
					fseek(dlg->pSrcFile, 0, SEEK_SET);

					if(INVALID_FILE_SIZE != fileLen)
					{
						dlg->srcFileByte = (unsigned char*)malloc(fileLen );
						memset(dlg->srcFileByte, 0, fileLen);
						fontGetSrcFileBytes(dlg->pSrcFile, dlg->srcFileByte, fileLen);

						#if 0
							{
								FILE *fpout;
								fopen_s(&fpout, "test.txt", "wb+");
								if(NULL != fpout)
								{
									fwrite(srcFileByte,1,fileLen,fpout);
									fclose(fpout);
									fpout = NULL;
								}
							}
						#endif

						//获取行列，并将非有效的数据滤掉，例如\r\n
						fontGetConvertConfig(dlg->srcFileByte, fileLen, &(dlg->column), &(dlg->row));
					}
					else
					{
						//debugList->AddString(_T("fileSize is err!\n"));
						MessageBox(NULL,_T("文件打开失败"), _T("注意"), MB_OK);
						TRACE("open file fail!\n");
					}
				}

				break;

				case USER_QUEUE_MSG_OUTPUT_FILE:
					fontOutputFile(dlg->pOutFile, dlg->srcFileByte, dlg->column, dlg->row, dlg->scanDirectFlag);
					MessageBox(NULL,_T("文件转换结束"), _T("成功"), MB_OK);
					break;
			default:
				break;
			}



			threadMsgQueue.pop();
		}
		Sleep(1000);
	}
	return 0;
}

