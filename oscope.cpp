#include <wx/wx.h>
#include <cmath>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

int index = 1;

std::vector<float>* voltages = new std::vector<float>;

constexpr double PI = 3.14159265358979323846;

float scale = 5.0;

float amplitude = 5.0;
float freq = 10.0;
std::string waveType = "sin";

float amplitude2 = 6.0;
float freq2 = 0.0;
std::string waveType2 = "sin";

bool isPaused = false;
bool isOn = false;

float period = 0.75;

double currentX = 0.0;

std::string programName = "UVille Oscilloscope";
std::string buttonName = "Pause";

struct cycleRate {
    int top = 7;
    int bottom = 8;
};

int samplingRate = 10; //In ms

void readVoltages(std::vector<float>* voltages, std::string filename) {
    std::string line = "";
    std::ifstream file(filename);
    for (int i = 0; i < 100000; ++i) {
        getline(file, line);
        voltages->push_back(stof(line));
    }
}

float wave(std::string type, float dub, float amp, float freq, float interval, int Vindex) {
    cycleRate cRate;
    amp = amp * interval;
    if (type == "sin") {
        return amp * std::sin(freq * dub);
    }
    else if (type == "cos") {
        return amp * std::cos(freq * dub);
    }
    else if (type == "square") {
        if ((int)(freq * dub) % 2 == 1) return amp;
        else return (-1.0 * amp);
    }
    else if (type == "dc") {
        return amp;
    }
    else if (type == "cycle") {
        if ((int)(freq * dub) % cRate.bottom < cRate.top) {
            return amp;
        }
        else return 0.0;
    }
    else {
        return 0.0;
    }
}

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
        SetBackgroundColour(*wxWHITE);
        timer = new wxTimer(this, wxID_ANY);
        Bind(wxEVT_TIMER, &MyFrame::OnTimer, this);
        timer->Start(samplingRate); // 50ms interval

        Bind(wxEVT_PAINT, &MyFrame::OnPaint, this);
    }

    void OnTimer(wxTimerEvent& event) {

        if(!isPaused) currentX += 0.1; // Increment x
        if (currentX > 100.0) currentX = 0.0; // Reset x if it exceeds 10
        Refresh();
    }

    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);
        Render(dc);
    }

    void Render(wxDC& dc) {
        int width, height;
        GetClientSize(&width, &height);

        // Clear background
        dc.SetBackground(*wxBLACK_BRUSH);
        dc.Clear();

        // Draw axes
        dc.SetPen(*wxYELLOW_PEN);
        dc.DrawLine(50, height / 2, width, height / 2); // X-axis
        dc.DrawLine(50, 0, 50, height); // Y-axis

        // Draw labels
        //dc.DrawText("-5", 15, height - 20);
        //dc.DrawLine(15, height - 14, 40, height -  14);
        //dc.DrawText("0", 40, height / 2 + 5);

        //Get number of segments
        int segments = scale / 0.5;                     //amplitude + 2.0;
        float interval = height / (2.0 * (float)segments);
        //double scale = 2.5;

        dc.SetTextForeground(*wxYELLOW);
        dc.DrawLine(40.0, height / 2.0, 60.0, height / 2.0);
        dc.DrawText("0", 30.0, height / 2.0 - 8.0);

        float j = 0.5;
        for (float i = (height / 2.0) - interval; i >= interval - 5.0; i -= interval) {
            dc.DrawLine(40.0, i, 60.0, i);
            dc.DrawText(std::to_string(j).substr(0, 3), 20.0, i - 8.8);
            j += 0.5;
        }
        j = -0.5;
        for (float i = (height / 2.0) + interval; i <= height - 5.0; i += interval) {
            dc.DrawLine(40.0, i, 60.0, i);
            dc.DrawText(std::to_string(j).substr(0, 4), 15.0, i - 8.8);
            j -= 0.5;
        }


        //Numbers
        /*double j = 0.0;
        for (double i = 0; i <= height / 2.0; i += 0.5) {
            dc.DrawText(std::to_string(i).substr(0, 3), 20.0, (height / 2.0) - ((j)*height / 15.25) - 8.0);
            dc.DrawLine(40.0, (height / 2.0) - ((j)*height / 15.25), 60.0, (height / 2.0) - ((j)*height / 15.25));
            j += 0.57;
        }
        //j = 0.57;
        for (double i = -0.5; i >= -6.5; i -= 0.5) {
            dc.DrawText(std::to_string(i).substr(0, 4), 15.0, (height / 2.0) + ((j)*height / 15.25) - 8.0);
            dc.DrawLine(40.0, (height / 2.0) + ((j)*height / 15.25), 60.0, (height / 2.0) + ((j)*height / 15.25));
            j += 0.57;
        }
        dc.DrawText(std::to_string(height), 50.0, height / 2 - 20.0);*/



        //dc.DrawText("5", 15, 5);

        // Set thicker pen for sine wave
        //wxPen thickPen(*wxBLUE, 2); // 3 is the thickness
        //dc.SetPen(thickPen);

        //// Draw manual signal
        //double prevY1 = voltages->at(index - 1);
        //for (int x = 1; x < width - 50; x += 1.0) {
        //    float y1 = voltages->at(index);
        //    float screenY1 = 10.0 * prevY1;
        //    float screenY2 = 10.0 * y1;
        //    dc.DrawLine(x + 50 - 1, (height / 2) - (prevY1 * 40.0), x + 50, (height / 2) - (y1 * 40.0));
        //    prevY1 = y1;
        //    if (!index < voltages->size() && !isPaused) {
        //        index++;
        //    }
        //}

        wxPen thickPen2(*wxGREEN, 1); // 3 is the thickness
        dc.SetPen(thickPen2);

        // Draw sine wave
        double prevY1 = wave(waveType, 1.2 * PI * currentX, amplitude, freq, interval, index - 1);
        //double prevY2 = wave(waveType2, 1.2 * PI * currentX, amplitude2, freq2, interval);
        for (int x = 1; x < width - 50; x++) {
            double newX1 = currentX + static_cast<double>(x) / static_cast<double>(width - 50);
            double y1 = wave(waveType, 1.2 * PI * newX1, amplitude, freq, interval, index);
            int screenY1 = height / 2 - static_cast<int>(prevY1);
            int screenY2 = height / 2 - static_cast<int>(y1);
            dc.DrawLine(x + 50 - 1, screenY1, x + 50, screenY2);
            prevY1 = y1;
        }
        //
        //wxPen thickPen3(*wxGREEN, 1); // 3 is the thickness
        //dc.SetPen(thickPen3);

        ////2nd Wave
        //// Draw sine wave
        //double prevY2 = wave(waveType2, 1.2 * PI * currentX, amplitude2, freq2, interval);
        //for (int x = 1; x < width - 50; x++) {
        //    double newX = currentX + static_cast<double>(x) / static_cast<double>(width - 50);
        //    double y = wave(waveType2, 1.2 * PI * newX, amplitude2, freq2, interval);
        //    int screenY1 = height / 2 - static_cast<int>(prevY2);
        //    int screenY2 = height / 2 - static_cast<int>(y);
        //    dc.DrawLine(x + 50 - 1, screenY1, x + 50, screenY2);
        //    prevY2 = y;
        //}
    }

    // Event handler for button click
    //void OnPauseButtonClick(wxCommandEvent& event) {
    //    //wxMessageBox("Button Clicked!", "Information", wxOK | wxICON_INFORMATION);
    //    if (isPaused == false) {
    //        isPaused = true;
    //        //pauseButton->
    //    }
    //    else {
    //        isPaused = false;
    //    }
    //}
    void ShowFrame() {
        Show(true);
    }

    void HideFrame() {
        Show(false);
    }


private:
    wxTimer* timer;
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        frame = new MyFrame(programName);

        pause_Button = new wxButton(frame, wxID_ANY, buttonName);
        pause_Button->Bind(wxEVT_BUTTON, &MyApp::OnPauseButtonClick, this);

        scaleUp_Button = new wxButton(frame, wxID_ANY, "Add");
        scaleUp_Button->Bind(wxEVT_BUTTON, &MyApp::addScale, this);

        scaleDown_Button = new wxButton(frame, wxID_ANY, "Sub");
        scaleDown_Button->Bind(wxEVT_BUTTON, &MyApp::subScale, this);

        changeWave = new wxButton(frame, wxID_ANY, "Change Wave");
        changeWave->Bind(wxEVT_BUTTON, &MyApp::changeWaveType, this);

        leftButton = new wxButton(frame, wxID_ANY, "Left");
        leftButton->Bind(wxEVT_BUTTON, &MyApp::shiftLeft, this);

        rightButton = new wxButton(frame, wxID_ANY, "Right");
        rightButton->Bind(wxEVT_BUTTON, &MyApp::shiftRight, this);

        wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
        //frameSizer->AddStretchSpacer(1);

        wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
        buttonsSizer->Add(pause_Button, 0, wxALL, 5);
        buttonsSizer->Add(scaleUp_Button, 0, wxALL, 5);
        buttonsSizer->Add(scaleDown_Button, 0, wxALL, 5);
        buttonsSizer->Add(changeWave, 0, wxALL, 5);
        buttonsSizer->Add(leftButton, 0, wxALL, 5);
        buttonsSizer->Add(rightButton, 0, wxALL, 5);


        frameSizer->AddStretchSpacer(1);
        frameSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT | wxALL, 5);


        frame->SetSizer(frameSizer);
        frame->Layout(); // Ensure layout is updated
        //frame->SetSizerAndFit(previousSize);

        //readVoltages(voltages, "C:/uVilleMagDump/test.txt");

        frame->Show(true);
        return true;
    }

    // Event handler for button click
    void OnPauseButtonClick(wxCommandEvent& event) {
        //wxMessageBox("Button Clicked!", "Information", wxOK | wxICON_INFORMATION);
        if (isPaused == false) {
            isPaused = true;
            pause_Button->SetLabelText("Start");
        }
        else {
            isPaused = false;
            pause_Button->SetLabelText("Pause");
        }
    }

    void addScale(wxCommandEvent& event) {
        scale += 1.0;
    }

    void subScale(wxCommandEvent& event) {
        scale -= 1.0;
    }

    void shiftRight(wxCommandEvent& event) {
        currentX -= 0.05;
    }

    void shiftLeft(wxCommandEvent& event) {
        currentX += 0.05;
    }

    void changeWaveType(wxCommandEvent& event) {
        if (waveType == "sin") waveType = "cos";
        else if (waveType == "cos") waveType = "square";
        else if (waveType == "square") waveType = "dc";
        else if (waveType == "dc") waveType = "sin";
    }

    // Override OnExit to perform cleanup when the application exits
    virtual int OnExit() override {
        // Perform cleanup tasks here

        // Return the exit code
        return wxApp::OnExit();
    }

    MyFrame* GetFrame() {
        return frame;
    }
private:
    MyFrame*  frame;
    wxButton* pause_Button;
    wxButton* scaleUp_Button;
    wxButton* scaleDown_Button;
    wxButton* changeWave;
    wxButton* leftButton;
    wxButton* rightButton;
};

//try {
wxIMPLEMENT_APP(MyApp);
//}
//catch (Exception e) {
//
//}

int main(int argc, char* argv[]) {
    // Initialize the wxWidgets application
    wxApp::SetInstance(new MyApp);
    wxEntryStart(argc, argv);

    // Get the frame instance
    MyApp* app = dynamic_cast<MyApp*>(wxApp::GetInstance());
    MyFrame* frame = app->GetFrame();

    // Show the frame
    frame->ShowFrame();

    // Event loop
    wxTheApp->OnRun();

    // Clean up
    wxEntryCleanup();
    return 0;
}



