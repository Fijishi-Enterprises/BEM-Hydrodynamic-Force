// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 - 2022, the BEMRosetta author and contributors
#ifndef _BEMRosetta_BEMRosetta_auxiliar_h_
#define _BEMRosetta_BEMRosetta_auxiliar_h_

class RichTextView2 : public RichTextView {
public:
	RichTextView2() {
		zoomlevel = 5;
		//Background(AdjustIfDark(Color(245, 245, 245)));
	}
	virtual void Layout() {
		RichTextView::Layout();
		PageWidth(int(zoomlevel*GetSize().cx));
	}
	double zoomlevel;
};

class BorderFrameDyn : public CtrlFrame {
public:
	void Init(int _thickness, Color _color) {
		thickness = _thickness;
		color = _color;
	}
	virtual void FrameLayout(Rect& r) {
		Size sz = r.GetSize();
		int n = thickness;
		if(sz.cx >= 2 * n && sz.cy >= 2 * n)
			r.Deflate(n);
	}
	virtual void FrameAddSize(Size& sz) {};//{sz += 2 * thickness;}
	virtual void FramePaint(Draw& draw, const Rect& r) {
		if (!show)
			return;
		Size sz = r.GetSize();
		if(sz.cx >= 2 * thickness && sz.cy >= 2 * thickness)
			DrawBorder(draw, r.left-1, r.top, r.Width()+2, r.Height(), color);
	}
	void Show(bool _show) 	{show = _show;}
	bool IsShown()			{return show;}
	
protected:
	bool show = false;
	int thickness;
	Color color;

	void DrawBorder(Draw& w, int x, int y, int cx, int cy, Color c) {
		int n = thickness;
		while(n--) {
			if(cx <= 0 || cy <= 0)
				break;
			DrawFrame(w, x, y, cx, cy, c, c, c, c);
			x += 1;
			y += 1;
			cx -= 2;
			cy -= 2;
		}
	}
};

class RectEnter {
public:
	void Init(int width = 2, Color color = LtBlue()) {
		frame.Init(width, color); 
	}
	void ShowFrame(bool show = true) {
		frame.Show(show);
		ctrl->RefreshFrame();
	}
	bool IsShown() {return frame.IsShown();}
	
	BorderFrameDyn frame;
	Function <void(RectEnter &)>WhenEnter;
	Ctrl *ctrl;
};

template <class T>
class UnderlineCtrl : public T {
public:
	UnderlineCtrl() {
		T::AddFrame(rectEnter.frame);
		rectEnter.ctrl = this;
		rectEnter.Init();
	}
	void Underline(double sec) {
		rectEnter.ShowFrame(true);
		timer.Set(-int(sec*1000), [&] {rectEnter.ShowFrame(false); timer.Kill();});
	}
	virtual void MouseEnter(Point, dword) {
		if (isMouseEnter) {
			rectEnter.ShowFrame(true);
			rectEnter.WhenEnter(rectEnter);
		}
	}
	bool IsShownFrame()			{return rectEnter.frame.IsShown();}
	RectEnter &GetRectEnter()	{return rectEnter;}
	
	bool isMouseEnter = true;
	
private:
	RectEnter rectEnter;
	TimeCallback timer;
};

class RectEnterSet {
public:
	typedef RectEnterSet CLASSNAME;
	
	void Add(RectEnter &ctrl, int width = 2, Color color = LtBlue()) {
		ctrl.Init(width, color);
		ctrls << &ctrl;
		ctrl.WhenEnter = [&](RectEnter &ctrl) {OnEnter(ctrl);};
	}
	void Remove(RectEnter &ctrl) {
		int id = Find(ctrl);
		if (id >= 0)
			ctrls.Remove(id);
	}
	void Set(RectEnter &ctrl) {
		int id = Find(ctrl);
		if (id >= 0)
			ctrls[id]->ShowFrame();
	}
	void OnEnter(RectEnter &ctrl) {
		for (int i = 0; i < ctrls.size(); ++i) { 
			if (&ctrl != ctrls[i]) 
				ctrls[i]->ShowFrame(false);
		}
		WhenEnter();
	}
	Function <void()>WhenEnter;

private:
	UVector<RectEnter *> ctrls;
	
	int Find(RectEnter &ctrl) {
		for (int i = 0; i < ctrls.size(); ++i) 
			if (&ctrl == ctrls[i])
				return i;
		return -1; 
	}
};


const Color &GetColorId(int id);

void GridCtrlToMatrixXd(Eigen::MatrixXd &mat, const GridCtrl &grid);
void MatrixXdToGridCtrl(GridCtrl &grid, const Eigen::MatrixXd &mat);
	
#endif
