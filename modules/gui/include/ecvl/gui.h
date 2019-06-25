#ifndef ECVL_GUI_H_
#define ECVL_GUI_H_

#include <wx/wx.h>

#undef _

#include "ecvl/core/image.h"

namespace ecvl {

/** @brief wxImagePanel creates a wxPanel to contain an Image.

*/
class wxImagePanel : public wxPanel
{
    wxImage image_;
    wxBitmap resized_;
    int w_, h_;
    void PaintEvent(wxPaintEvent & evt);
    void OnSize(wxSizeEvent& event);
    void Render(wxDC& dc);

public:
    wxImagePanel(wxFrame* parent) : wxPanel(parent) {}      
    void SetImage(const wxImage& img);

    DECLARE_EVENT_TABLE()
};

/** @brief ShowApp is a custom wxApp which allows you to visualize an ECVL Image.

*/
class ShowApp : public wxApp
{
    Image img_;      /**< Image to be shown. */

public:
    /** @brief Initialization function. Starts the main loop of the application.

        The OnInit() function creates a wxFrame which has the width and the height of the Image that has to be shown.
        It also creates the wxImagePanel which contains the frame and employs the conversion from Image to
        wxImage.
        It set the wxImage in the frame and starts the main loop of the ShowApp. 
    */
    bool OnInit();

    /** @brief Constructor.

        The constructor creates a ShowApp initializing its Image with the given input Image.
    */
    ShowApp(const Image &img) : img_{ img } {}
};

/** @brief Displays an Image.

The ImShow function instantiates a ShowApp and starts it with a wxEntry() call.
The image is shown with its original size. 

@param[in] img Image to be shown.

*/
void ImShow(const Image& img);

/** @brief Convert an ECVL Image into a wxImage.

@param[in] img Input ECVL Image.

@return wxImage.
*/

wxImage WxFromImg(Image& img);

/** @brief Convert a wxImage into an ECVL Image.

@param[in] wx Input wxImage.

@return ECVL Image.
*/

Image ImgFromWx(const wxImage& wx);
} // namespace ecvl

#endif // ECVL_GUI_H_