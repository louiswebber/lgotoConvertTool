
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
	ON_BN_CLICKED(IDC_BUTTON3, &CfontLibToolsAppDlg::OnBnClickedButton3)
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
	pOutFile	= NULL;
	memset(srcFileName, 0, 400);
	memset(outFileName, 0, 400);
	srcFileByte	= NULL;
	outFileByte	= NULL;
	column		= 0;
	row			= 0;
	scanDirectFlag	= USER_DIRECT_R2L;//初始化扫描方向默认从右到左
	SetDlgItemText(IDC_BUTTON3, _T("从右向左"));
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
		//获取源文件名
		CString srcName = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT1, srcName);

		int lenth	= 0;
		//转换源文件名unicode到ascii码
		lenth	= WideCharToMultiByte(CP_ACP, 0, srcName, -1, NULL, 0, NULL, 0);
		memset(srcFileName, 0, sizeof(srcFileName));
		WideCharToMultiByte(CP_ACP, 0, srcName, -1, srcFileName, lenth, NULL, 0);
		//生成输出文件路径 和 文件名
		memset(outFileName, 0, sizeof(outFileName));
		memcpy(outFileName, srcFileName, sizeof(outFileName));

		{
			char *pAddr1 = outFileName, *pAddr2 = outFileName;

			while(pAddr1 = strstr(pAddr1, "\\"))
			{
				pAddr1 ++;
				pAddr2 = pAddr1;
			}

			memset(pAddr2, 0, 400 - (pAddr2 - outFileName));
			memcpy(pAddr2, "demo_cpp.asm", strlen("demo_cpp.asm"));
		}
		threadMsgQueue.push(USER_QUEUE_MSG_OPEN_FILE);
	}
}

void CfontLibToolsAppDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	threadMsgQueue.push(USER_QUEUE_MSG_OUTPUT_FILE);

}

int fontGetSrcFileBytes(FILE* fp, unsigned char* pOutBuf, DWORD len)
{
	if((NULL == pOutBuf) || (NULL == fp))
	{
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

	memset(pAddrDst, 0, pAddrSrc - pAddrDst);
	return 0;
}

int fontOutputFile(char* outFileByte, unsigned char *srcBuf, DWORD column, DWORD row, char dir)
{
	DWORD lenth	= column * row;
	DWORD i,j,k,data;
	unsigned char *pSrcAddr;
	char outBuf[20],h2StrBuf[8], *pOutBufAddr, tempData;

	if((NULL == outFileByte) || (NULL == srcBuf))
	{
		MessageBox(NULL,_T("非法缓存地址"), _T("注意"), MB_OK);
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
			memset(outBuf, 0, 20);
			strcat_s(outBuf, 20, "lgoto  ");
			pOutBufAddr	= outBuf + strlen(outBuf);
			memset(pOutBufAddr, 0, 20 - strlen(outBuf));
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
			strcat_s(outBuf, 20, "h\r\n");
			pOutBufAddr	= outBuf + strlen(outBuf);
			memset(pOutBufAddr, 0, 20 - strlen(outBuf));
			memcpy(outFileByte + (column - i) * 20, outBuf, 20);
		}
	}
	else if(USER_DIRECT_L2R == dir)
	{
		for(i = 0; i < column; i ++)
		{
			data	= 0;
			for(j = row ; j > 0; j --)
			{
				pSrcAddr = srcBuf + (column * (j - 1) + i) * USER_STRING_BYTES;
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
			memset(outBuf, 0, 20);
			strcat_s(outBuf, 20, "lgoto  ");
			pOutBufAddr	= outBuf + strlen(outBuf);
			memset(pOutBufAddr, 0, 20 - strlen(outBuf));
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
			strcat_s(outBuf, 20, "h\r\n");
			pOutBufAddr	= outBuf + strlen(outBuf);
			memset(pOutBufAddr, 0, 20 - strlen(outBuf));
			memcpy(outFileByte + i * 20, outBuf, 20);
		}
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
					break;

				case USER_QUEUE_MSG_OUTPUT_FILE:
					{
						DWORD fileLen = 0;

						fopen_s(&(dlg->pSrcFile), dlg->srcFileName, "rb");//打开源文件

						if(NULL == dlg->pSrcFile)
						{
							MessageBox(NULL,_T("文件打开失败"), _T("注意"), MB_OK);
							break;
						}

						//获取文件内容长度
						fseek(dlg->pSrcFile, 0, SEEK_END);
						fileLen	= ftell(dlg->pSrcFile);
						fseek(dlg->pSrcFile, 0, SEEK_SET);

						if(INVALID_FILE_SIZE == fileLen)
						{
							if(dlg->pSrcFile)
							{
								dlg->pSrcFile = NULL;
							}
							MessageBox(NULL,_T("文件无内容"), _T("注意"), MB_OK);
							break;
						}

						//申请内存用于读取源文件
						dlg->srcFileByte = (unsigned char*)malloc(fileLen);
						memset(dlg->srcFileByte, 0, fileLen);
						//读取源文件内容
						fontGetSrcFileBytes(dlg->pSrcFile, dlg->srcFileByte, fileLen);
						//关闭源文件
						if(dlg->pSrcFile)
						{
							fclose(dlg->pSrcFile);
							dlg->pSrcFile = NULL;
						}
						//获取行列，并将非有效的数据滤掉，例如\r\n
						fontGetConvertConfig(dlg->srcFileByte, fileLen, &(dlg->column), &(dlg->row));
						//申请内存用于存储输出文件
						dlg->outFileByte = (char*)malloc(20 * dlg->column);
						memset(dlg->outFileByte, 0, 20 * dlg->column);
						fontOutputFile(dlg->outFileByte, dlg->srcFileByte, dlg->column, dlg->row,dlg->scanDirectFlag);
						//释放源文件内容内存
						if(dlg->srcFileByte)
						{
							free(dlg->srcFileByte);
							dlg->srcFileByte = NULL;
						}
					}
					fopen_s(&(dlg->pOutFile), dlg->outFileName, "wb+");
					if(NULL == dlg->pOutFile)//打开源文件
					{
						MessageBox(NULL,_T("输出文件创建失败"), _T("注意"), MB_OK);
						break;
					}

					{
						DWORD i;
						for(i = 0; i < dlg->column; i ++)
						{
							fwrite(dlg->outFileByte + 20 * i, 1, strlen(dlg->outFileByte + 20 * i), dlg->pOutFile);
						}
					}

					if(dlg->outFileByte)
					{
						free(dlg->outFileByte);
						dlg->outFileByte = NULL;
					}

					if(dlg->pOutFile)
					{
						fclose(dlg->pOutFile);
						dlg->pOutFile = NULL;
					}
					MessageBox(NULL,_T("文件转换结束"), _T("成功"), MB_OK);
					break;
				default:
					break;
			}

			threadMsgQueue.pop();
		}
		Sleep(100);
	}
	return 0;
}



void CfontLibToolsAppDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	if(USER_DIRECT_R2L == scanDirectFlag)
	{
		SetDlgItemText(IDC_BUTTON3, _T("从左向右"));
		scanDirectFlag = USER_DIRECT_L2R;
	}
	else if(USER_DIRECT_L2R == scanDirectFlag)
	{
		SetDlgItemText(IDC_BUTTON3, _T("从右向左"));
		scanDirectFlag = USER_DIRECT_R2L;
	}
}
