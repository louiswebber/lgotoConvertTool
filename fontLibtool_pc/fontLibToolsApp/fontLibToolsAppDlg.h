
// fontLibToolsAppDlg.h : ͷ�ļ�
//

#pragma once

using namespace std;
// CfontLibToolsAppDlg �Ի���
class CfontLibToolsAppDlg : public CDialogEx
{
// ����
public:
	CfontLibToolsAppDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FONTLIBTOOLSAPP_DIALOG };


	HANDLE fontThreadHandle;
	FILE* pSrcFile;
	FILE* pOutFile;
	CString srcFileName;
	CString outFileName;
	unsigned char *srcFileByte;

	DWORD column;
	DWORD row;
	char scanDirectFlag;
	//static DWORD WINAPI fontThreadProc(LPVOID lpParameter);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton2();
};
