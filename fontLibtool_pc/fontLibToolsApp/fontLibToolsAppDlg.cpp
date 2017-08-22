
// fontLibToolsAppDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CfontLibToolsAppDlg �Ի���

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


// CfontLibToolsAppDlg ��Ϣ�������

BOOL CfontLibToolsAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	hwnd	= m_hWnd;//��ȡ�Ի�����
	pwnd	= FromHandle(hwnd);//���ݾ��ת���ɶԻ���ָ��
	pSrcFile	= NULL;
	pOutFile	= NULL;
	memset(srcFileName, 0, 400);
	memset(outFileName, 0, 400);
	srcFileByte	= NULL;
	outFileByte	= NULL;
	column		= 0;
	row			= 0;
	scanDirectFlag	= USER_DIRECT_R2L;//��ʼ��ɨ�跽��Ĭ�ϴ��ҵ���
	SetDlgItemText(IDC_BUTTON3, _T("��������"));
	fontThreadHandle = CreateThread(NULL, 0, fontThreadProc, NULL, 0, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CfontLibToolsAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CfontLibToolsAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CfontLibToolsAppDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog fileDlg(TRUE);// TRUE for FileOpen, FALSE for FileSaveAs
	fileDlg.m_ofn.lpstrTitle=_T("ѡ��Ҫ������ļ�");
	fileDlg.m_ofn.lpstrFilter=_T("asm�ļ�(*.asm)\0*.asm\0�����ļ�(*.*)\0*.*\0");
	if( IDOK ==fileDlg.DoModal())
	{
		//��ȡԴ�ļ���
		CString srcName = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT1, srcName);

		int lenth	= 0;
		//ת��Դ�ļ���unicode��ascii��
		lenth	= WideCharToMultiByte(CP_ACP, 0, srcName, -1, NULL, 0, NULL, 0);
		memset(srcFileName, 0, sizeof(srcFileName));
		WideCharToMultiByte(CP_ACP, 0, srcName, -1, srcFileName, lenth, NULL, 0);
		//��������ļ�·�� �� �ļ���
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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
		MessageBox(NULL,_T("�������һ�������Ƿ��㹻��"), _T("ע��"), MB_OK);
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
		MessageBox(NULL,_T("�Ƿ������ַ"), _T("ע��"), MB_OK);
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
	CfontLibToolsAppDlg *dlg	= (CfontLibToolsAppDlg*)pwnd;//��ȡ�Ի���ָ��
	//HWND hwndproc = GetDlgItem(hwnd,IDD_FONTLIBTOOLSAPP_DIALOG);//��ȡ�Ի�����

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

						fopen_s(&(dlg->pSrcFile), dlg->srcFileName, "rb");//��Դ�ļ�

						if(NULL == dlg->pSrcFile)
						{
							MessageBox(NULL,_T("�ļ���ʧ��"), _T("ע��"), MB_OK);
							break;
						}

						//��ȡ�ļ����ݳ���
						fseek(dlg->pSrcFile, 0, SEEK_END);
						fileLen	= ftell(dlg->pSrcFile);
						fseek(dlg->pSrcFile, 0, SEEK_SET);

						if(INVALID_FILE_SIZE == fileLen)
						{
							if(dlg->pSrcFile)
							{
								dlg->pSrcFile = NULL;
							}
							MessageBox(NULL,_T("�ļ�������"), _T("ע��"), MB_OK);
							break;
						}

						//�����ڴ����ڶ�ȡԴ�ļ�
						dlg->srcFileByte = (unsigned char*)malloc(fileLen);
						memset(dlg->srcFileByte, 0, fileLen);
						//��ȡԴ�ļ�����
						fontGetSrcFileBytes(dlg->pSrcFile, dlg->srcFileByte, fileLen);
						//�ر�Դ�ļ�
						if(dlg->pSrcFile)
						{
							fclose(dlg->pSrcFile);
							dlg->pSrcFile = NULL;
						}
						//��ȡ���У���������Ч�������˵�������\r\n
						fontGetConvertConfig(dlg->srcFileByte, fileLen, &(dlg->column), &(dlg->row));
						//�����ڴ����ڴ洢����ļ�
						dlg->outFileByte = (char*)malloc(20 * dlg->column);
						memset(dlg->outFileByte, 0, 20 * dlg->column);
						fontOutputFile(dlg->outFileByte, dlg->srcFileByte, dlg->column, dlg->row,dlg->scanDirectFlag);
						//�ͷ�Դ�ļ������ڴ�
						if(dlg->srcFileByte)
						{
							free(dlg->srcFileByte);
							dlg->srcFileByte = NULL;
						}
					}
					fopen_s(&(dlg->pOutFile), dlg->outFileName, "wb+");
					if(NULL == dlg->pOutFile)//��Դ�ļ�
					{
						MessageBox(NULL,_T("����ļ�����ʧ��"), _T("ע��"), MB_OK);
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
					MessageBox(NULL,_T("�ļ�ת������"), _T("�ɹ�"), MB_OK);
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(USER_DIRECT_R2L == scanDirectFlag)
	{
		SetDlgItemText(IDC_BUTTON3, _T("��������"));
		scanDirectFlag = USER_DIRECT_L2R;
	}
	else if(USER_DIRECT_L2R == scanDirectFlag)
	{
		SetDlgItemText(IDC_BUTTON3, _T("��������"));
		scanDirectFlag = USER_DIRECT_R2L;
	}
}
