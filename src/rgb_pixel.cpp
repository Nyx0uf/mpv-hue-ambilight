#include "rgb_pixel.h"


double __normalized_component_values[256] = {
0.0,
0.00392156862745,
0.0078431372549,
0.0117647058824,
0.0156862745098,
0.0196078431373,
0.0235294117647,
0.0274509803922,
0.0313725490196,
0.0352941176471,
0.0392156862745,
0.043137254902,
0.0470588235294,
0.0509803921569,
0.0549019607843,
0.0588235294118,
0.0627450980392,
0.0666666666667,
0.0705882352941,
0.0745098039216,
0.078431372549,
0.0823529411765,
0.0862745098039,
0.0901960784314,
0.0941176470588,
0.0980392156863,
0.101960784314,
0.105882352941,
0.109803921569,
0.113725490196,
0.117647058824,
0.121568627451,
0.125490196078,
0.129411764706,
0.133333333333,
0.137254901961,
0.141176470588,
0.145098039216,
0.149019607843,
0.152941176471,
0.156862745098,
0.160784313725,
0.164705882353,
0.16862745098,
0.172549019608,
0.176470588235,
0.180392156863,
0.18431372549,
0.188235294118,
0.192156862745,
0.196078431373,
0.2,
0.203921568627,
0.207843137255,
0.211764705882,
0.21568627451,
0.219607843137,
0.223529411765,
0.227450980392,
0.23137254902,
0.235294117647,
0.239215686275,
0.243137254902,
0.247058823529,
0.250980392157,
0.254901960784,
0.258823529412,
0.262745098039,
0.266666666667,
0.270588235294,
0.274509803922,
0.278431372549,
0.282352941176,
0.286274509804,
0.290196078431,
0.294117647059,
0.298039215686,
0.301960784314,
0.305882352941,
0.309803921569,
0.313725490196,
0.317647058824,
0.321568627451,
0.325490196078,
0.329411764706,
0.333333333333,
0.337254901961,
0.341176470588,
0.345098039216,
0.349019607843,
0.352941176471,
0.356862745098,
0.360784313725,
0.364705882353,
0.36862745098,
0.372549019608,
0.376470588235,
0.380392156863,
0.38431372549,
0.388235294118,
0.392156862745,
0.396078431373,
0.4,
0.403921568627,
0.407843137255,
0.411764705882,
0.41568627451,
0.419607843137,
0.423529411765,
0.427450980392,
0.43137254902,
0.435294117647,
0.439215686275,
0.443137254902,
0.447058823529,
0.450980392157,
0.454901960784,
0.458823529412,
0.462745098039,
0.466666666667,
0.470588235294,
0.474509803922,
0.478431372549,
0.482352941176,
0.486274509804,
0.490196078431,
0.494117647059,
0.498039215686,
0.501960784314,
0.505882352941,
0.509803921569,
0.513725490196,
0.517647058824,
0.521568627451,
0.525490196078,
0.529411764706,
0.533333333333,
0.537254901961,
0.541176470588,
0.545098039216,
0.549019607843,
0.552941176471,
0.556862745098,
0.560784313725,
0.564705882353,
0.56862745098,
0.572549019608,
0.576470588235,
0.580392156863,
0.58431372549,
0.588235294118,
0.592156862745,
0.596078431373,
0.6,
0.603921568627,
0.607843137255,
0.611764705882,
0.61568627451,
0.619607843137,
0.623529411765,
0.627450980392,
0.63137254902,
0.635294117647,
0.639215686275,
0.643137254902,
0.647058823529,
0.650980392157,
0.654901960784,
0.658823529412,
0.662745098039,
0.666666666667,
0.670588235294,
0.674509803922,
0.678431372549,
0.682352941176,
0.686274509804,
0.690196078431,
0.694117647059,
0.698039215686,
0.701960784314,
0.705882352941,
0.709803921569,
0.713725490196,
0.717647058824,
0.721568627451,
0.725490196078,
0.729411764706,
0.733333333333,
0.737254901961,
0.741176470588,
0.745098039216,
0.749019607843,
0.752941176471,
0.756862745098,
0.760784313725,
0.764705882353,
0.76862745098,
0.772549019608,
0.776470588235,
0.780392156863,
0.78431372549,
0.788235294118,
0.792156862745,
0.796078431373,
0.8,
0.803921568627,
0.807843137255,
0.811764705882,
0.81568627451,
0.819607843137,
0.823529411765,
0.827450980392,
0.83137254902,
0.835294117647,
0.839215686275,
0.843137254902,
0.847058823529,
0.850980392157,
0.854901960784,
0.858823529412,
0.862745098039,
0.866666666667,
0.870588235294,
0.874509803922,
0.878431372549,
0.882352941176,
0.886274509804,
0.890196078431,
0.894117647059,
0.898039215686,
0.901960784314,
0.905882352941,
0.909803921569,
0.913725490196,
0.917647058824,
0.921568627451,
0.925490196078,
0.929411764706,
0.933333333333,
0.937254901961,
0.941176470588,
0.945098039216,
0.949019607843,
0.952941176471,
0.956862745098,
0.960784313725,
0.964705882353,
0.96862745098,
0.972549019608,
0.976470588235,
0.980392156863,
0.98431372549,
0.988235294118,
0.992156862745,
0.996078431373,
1.0};


rgb_pixel_t::rgb_pixel_t(void)
{
	r = NYX_MIN_PIXEL_COMPONENT_VALUE;
	g = NYX_MIN_PIXEL_COMPONENT_VALUE;
	b = NYX_MIN_PIXEL_COMPONENT_VALUE;
}

rgb_pixel_t::rgb_pixel_t(const uint8_t red, const uint8_t green, const uint8_t blue)
{
	r = red;
	g = green;
	b = blue;
}

rgb_pixel_t::rgb_pixel_t(const rgb_pixel_t& px)
{
	this->r = px.r;
	this->g = px.g;
	this->b = px.b;
}

bool rgb_pixel_t::is_dark(void)const
{
	const double lum = 0.2126 * __normalized_component_values[r] + 0.7152 * __normalized_component_values[g] + 0.0722 * __normalized_component_values[b];
	return (lum < 0.33);
}
