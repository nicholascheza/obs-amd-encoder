/*
MIT License

Copyright (c) 2016-2017

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "amf-encoder-h264.h"
#include "utility.h"
#include <inttypes.h>

#define QUICK_THROW_ERROR(format, ...) {\
	QUICK_FORMAT_MESSAGE(errMsg, __FUNCTION_NAME__ format, \
		m_UniqueId,  ##__VA_ARGS__, \
		m_AMF->GetTrace()->GetResultText(res), res); \
	throw std::exception(errMsg.c_str()); \
}

#define PREFIX "[H264]<Id: %lld> "

using namespace Plugin;
using namespace Plugin::AMD;
using namespace Utility;

Plugin::AMD::EncoderH264::EncoderH264(
	std::shared_ptr<API::IAPI> videoAPI, API::Adapter videoAdapter,
	bool useOpenCLSubmission, bool useOpenCLConversion,
	ColorFormat colorFormat, ColorSpace colorSpace, bool fullRangeColor,
	bool useAsyncQueue, size_t asyncQueueSize)
	: Encoder(Codec::AVC,
		videoAPI, videoAdapter,
		useOpenCLSubmission, useOpenCLConversion,
		colorFormat, colorSpace, fullRangeColor,
		useAsyncQueue, asyncQueueSize) {
	AMFTRACECALL;
	this->SetUsage(Usage::Transcoding);

	AMF_RESULT res = res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_FULL_RANGE_COLOR, m_FullColorRange);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg,
			PREFIX "Failed to set encoder color range, error %ls (code %d)",
			m_UniqueId,
			m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::EncoderH264::~EncoderH264() {
	AMFTRACECALL;
}

// Properties - Initialization
std::vector<Usage> Plugin::AMD::EncoderH264::CapsUsage() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_USAGE, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	std::vector<Usage> ret;
	for (const amf::AMFEnumDescriptionEntry* enm = var->pEnumDescription; enm->name != nullptr; enm++) {
		ret.push_back(Utility::UsageFromAMFH264((AMF_VIDEO_ENCODER_USAGE_ENUM)enm->value));
	}
	return ret;
}

void Plugin::AMD::EncoderH264::SetUsage(Usage v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_USAGE, Utility::UsageToAMFH264(v));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, Utility::UsageToString(v), m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::Usage Plugin::AMD::EncoderH264::GetUsage() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_USAGE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return Utility::UsageFromAMFH264((AMF_VIDEO_ENCODER_USAGE_ENUM)e);
}

// Properties - Static
std::vector<QualityPreset> Plugin::AMD::EncoderH264::CapsQualityPreset() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_QUALITY_PRESET, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	std::vector<QualityPreset> ret;
	for (const amf::AMFEnumDescriptionEntry* enm = var->pEnumDescription; enm->name != nullptr; enm++) {
		ret.push_back(Utility::QualityPresetFromAMFH264((AMF_VIDEO_ENCODER_QUALITY_PRESET_ENUM)enm->value));
	}
	return ret;
}

void Plugin::AMD::EncoderH264::SetQualityPreset(QualityPreset v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_QUALITY_PRESET,
		Utility::QualityPresetToAMFH264(v));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, Utility::QualityPresetToString(v), m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::QualityPreset Plugin::AMD::EncoderH264::GetQualityPreset() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_QUALITY_PRESET, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return Utility::QualityPresetFromAMFH264((AMF_VIDEO_ENCODER_QUALITY_PRESET_ENUM)e);
}

std::vector<Profile> Plugin::AMD::EncoderH264::CapsProfile() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_PROFILE, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	std::vector<Profile> ret;
	for (const amf::AMFEnumDescriptionEntry* enm = var->pEnumDescription; enm->name != nullptr; enm++) {
		ret.push_back(Utility::ProfileFromAMFH264((AMF_VIDEO_ENCODER_PROFILE_ENUM)enm->value));
	}
	return ret;
}

void Plugin::AMD::EncoderH264::SetProfile(Profile v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_PROFILE,
		Utility::ProfileToAMFH264(v));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, Utility::ProfileToString(v), m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::Profile Plugin::AMD::EncoderH264::GetProfile() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_PROFILE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return Utility::ProfileFromAMFH264((AMF_VIDEO_ENCODER_PROFILE_ENUM)e);
}

std::vector<ProfileLevel> Plugin::AMD::EncoderH264::CapsProfileLevel() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_PROFILE_LEVEL, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	std::vector<ProfileLevel> ret;
	for (const amf::AMFEnumDescriptionEntry* enm = var->pEnumDescription; enm->name != nullptr; enm++) {
		ret.push_back((ProfileLevel)enm->value);
	}
	return ret;
}

void Plugin::AMD::EncoderH264::SetProfileLevel(ProfileLevel v) {
	AMFTRACECALL;;

	if (v == ProfileLevel::Automatic)
		v = Utility::H264ProfileLevel(m_Resolution, m_FrameRate);

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_PROFILE_LEVEL,
		(int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %lld, error %ls (code %d)",
			m_UniqueId, (int64_t)v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::ProfileLevel Plugin::AMD::EncoderH264::GetProfileLevel() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_PROFILE_LEVEL, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (ProfileLevel)e;
}

std::pair<uint64_t, uint64_t> Plugin::AMD::EncoderH264::CapsMaximumReferenceFrames() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_MAX_NUM_REFRAMES, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair(var->minValue.int64Value, var->maxValue.int64Value);
}

void Plugin::AMD::EncoderH264::SetMaximumReferenceFrames(uint64_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_MAX_NUM_REFRAMES, v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %lld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint64_t Plugin::AMD::EncoderH264::GetMaximumReferenceFrames() {
	AMFTRACECALL;

	uint64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_MAX_NUM_REFRAMES, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, __FUNCTION_NAME__ PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> Plugin::AMD::EncoderH264::CapsResolution() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_FRAMESIZE, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair(
		std::make_pair(var->minValue.sizeValue.width, var->maxValue.sizeValue.width),
		std::make_pair(var->minValue.sizeValue.height, var->maxValue.sizeValue.height)
	);
}

void Plugin::AMD::EncoderH264::SetResolution(std::pair<uint32_t, uint32_t> v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_FRAMESIZE, ::AMFConstructSize(v.first, v.second));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ldx%ld, error %ls (code %d)",
			m_UniqueId, v.first, v.second, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	m_Resolution.first = v.first;
	m_Resolution.second = v.second;
}

std::pair<uint32_t, uint32_t> Plugin::AMD::EncoderH264::GetResolution() {
	AMFTRACECALL;

	AMFSize e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_FRAMESIZE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	m_Resolution.first = e.width;
	m_Resolution.second = e.height;
	return std::make_pair(e.width, e.height);
}

void Plugin::AMD::EncoderH264::SetAspectRatio(std::pair<uint32_t, uint32_t> v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_ASPECT_RATIO, ::AMFConstructRatio(v.first, v.second));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld:%ld, error %ls (code %d)",
			m_UniqueId, v.first, v.second, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

std::pair<uint32_t, uint32_t> Plugin::AMD::EncoderH264::GetAspectRatio() {
	AMFTRACECALL;

	AMFRatio e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_ASPECT_RATIO, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return std::make_pair(e.num, e.den);
}

void Plugin::AMD::EncoderH264::SetFrameRate(std::pair<uint32_t, uint32_t> v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_FRAMERATE, ::AMFConstructRate(v.first, v.second));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld/%ld, error %ls (code %d)",
			m_UniqueId, v.first, v.second, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	m_FrameRate = std::make_pair(v.first, v.second);
	UpdateFrameRateValues();
}

std::pair<uint32_t, uint32_t> Plugin::AMD::EncoderH264::GetFrameRate() {
	AMFTRACECALL;

	AMFRate e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_FRAMERATE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "Unable to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	m_FrameRate = std::make_pair(e.num, e.den);
	UpdateFrameRateValues();
	return m_FrameRate;
}

std::vector<CodingType> Plugin::AMD::EncoderH264::CapsCodingType() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_CABAC_ENABLE, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	std::vector<CodingType> ret;
	for (const amf::AMFEnumDescriptionEntry* enm = var->pEnumDescription; enm->name != nullptr; enm++) {
		ret.push_back(Utility::CodingTypeFromAMFH264((AMF_VIDEO_ENCODER_CODING_ENUM)enm->value));
	}
	return ret;
}

void Plugin::AMD::EncoderH264::SetCodingType(CodingType v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_CABAC_ENABLE, Utility::CodingTypeToAMFH264(v));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, Utility::CodingTypeToString(v), m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::CodingType Plugin::AMD::EncoderH264::GetCodingType() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_CABAC_ENABLE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "Unable to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return Utility::CodingTypeFromAMFH264((AMF_VIDEO_ENCODER_CODING_ENUM)e);
}

std::pair<uint32_t, uint32_t> Plugin::AMD::EncoderH264::CapsMaximumLongTermReferenceFrames() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_MAX_LTR_FRAMES, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair((uint32_t)var->minValue.int64Value, (uint32_t)var->maxValue.int64Value);
}

void Plugin::AMD::EncoderH264::SetMaximumLongTermReferenceFrames(uint32_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_MAX_LTR_FRAMES, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint32_t Plugin::AMD::EncoderH264::GetMaximumLongTermReferenceFrames() {
	AMFTRACECALL;

	int64_t e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_MAX_LTR_FRAMES, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint32_t)e;
}

// Properties - Dynamic
std::vector<RateControlMethod> Plugin::AMD::EncoderH264::CapsRateControlMethod() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_RATE_CONTROL_METHOD, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	std::vector<RateControlMethod> ret;
	for (const amf::AMFEnumDescriptionEntry* enm = var->pEnumDescription; enm->name != nullptr; enm++) {
		ret.push_back(Utility::RateControlMethodFromAMFH264((AMF_VIDEO_ENCODER_RATE_CONTROL_METHOD_ENUM)enm->value));
	}
	return ret;
}

void Plugin::AMD::EncoderH264::SetRateControlMethod(RateControlMethod v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_RATE_CONTROL_METHOD, Utility::RateControlMethodToAMFH264(v));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, Utility::RateControlMethodToString(v), m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::RateControlMethod Plugin::AMD::EncoderH264::GetRateControlMethod() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_RATE_CONTROL_METHOD, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return Utility::RateControlMethodFromAMFH264((AMF_VIDEO_ENCODER_RATE_CONTROL_METHOD_ENUM)e);
}

std::vector<PrePassMode> Plugin::AMD::EncoderH264::CapsPrePassMode() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_RATE_CONTROL_PREANALYSIS_ENABLE, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	std::vector<PrePassMode> ret;
	for (const amf::AMFEnumDescriptionEntry* enm = var->pEnumDescription; enm->name != nullptr; enm++) {
		ret.push_back(Utility::PrePassModeFromAMFH264((AMF_VIDEO_ENCODER_PREENCODE_MODE_ENUM)enm->value));
	}
	return ret;
}

void Plugin::AMD::EncoderH264::SetPrePassMode(PrePassMode v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_RATE_CONTROL_PREANALYSIS_ENABLE, Utility::PrePassModeToAMFH264(v));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, Utility::PrePassModeToString(v), m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::PrePassMode Plugin::AMD::EncoderH264::GetPrePassMode() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_RATE_CONTROL_PREANALYSIS_ENABLE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "Unable to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return Utility::PrePassModeFromAMFH264((AMF_VIDEO_ENCODER_PREENCODE_MODE_ENUM)e);
}

void Plugin::AMD::EncoderH264::SetVarianceBasedAdaptiveQuantizationEnabled(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_ENABLE_VBAQ, v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::IsVarianceBasedAdaptiveQuantizationEnabled() {
	AMFTRACECALL;

	bool e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_ENABLE_VBAQ, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetFrameSkippingEnabled(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_RATE_CONTROL_SKIP_FRAME_ENABLE, v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::IsFrameSkippingEnabled() {
	AMFTRACECALL;

	bool e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_RATE_CONTROL_SKIP_FRAME_ENABLE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetEnforceHRDEnabled(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_ENFORCE_HRD, v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::IsEnforceHRDEnabled() {
	AMFTRACECALL;

	bool e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_ENFORCE_HRD, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetFillerDataEnabled(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_FILLER_DATA_ENABLE, v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::IsFillerDataEnabled() {
	AMFTRACECALL;

	bool e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_FILLER_DATA_ENABLE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetQPMinimum(uint8_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_MIN_QP, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %d, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint8_t Plugin::AMD::EncoderH264::GetQPMinimum() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_MIN_QP, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint8_t)e;
}

void Plugin::AMD::EncoderH264::SetQPMaximum(uint8_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_MAX_QP, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %d, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint8_t Plugin::AMD::EncoderH264::GetQPMaximum() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_MAX_QP, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint8_t)e;
}

std::pair<uint64_t, uint64_t> Plugin::AMD::EncoderH264::CapsTargetBitrate() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_TARGET_BITRATE, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair(var->minValue.int64Value, var->maxValue.int64Value);
}

void Plugin::AMD::EncoderH264::SetTargetBitrate(uint64_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_TARGET_BITRATE, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %lld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint64_t Plugin::AMD::EncoderH264::GetTargetBitrate() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_TARGET_BITRATE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

std::pair<uint64_t, uint64_t> Plugin::AMD::EncoderH264::CapsPeakBitrate() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_PEAK_BITRATE, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair(var->minValue.int64Value, var->maxValue.int64Value);
}

void Plugin::AMD::EncoderH264::SetPeakBitrate(uint64_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_PEAK_BITRATE, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %lld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint64_t Plugin::AMD::EncoderH264::GetPeakBitrate() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_PEAK_BITRATE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetIFrameQP(uint8_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_QP_I, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %d, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint8_t Plugin::AMD::EncoderH264::GetIFrameQP() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_QP_I, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint8_t)e;
}

void Plugin::AMD::EncoderH264::SetPFrameQP(uint8_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_QP_P, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %d, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint8_t Plugin::AMD::EncoderH264::GetPFrameQP() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_QP_P, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint8_t)e;
}

void Plugin::AMD::EncoderH264::SetBFrameQP(uint8_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_QP_B, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %d, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint8_t Plugin::AMD::EncoderH264::GetBFrameQP() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_QP_B, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint8_t)e;
}

void Plugin::AMD::EncoderH264::SetMaximumAccessUnitSize(uint32_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_MAX_AU_SIZE, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint32_t Plugin::AMD::EncoderH264::GetMaximumAccessUnitSize() {
	AMFTRACECALL;

	int64_t e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_MAX_AU_SIZE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint32_t)e;
}

std::pair<uint64_t, uint64_t> Plugin::AMD::EncoderH264::CapsVBVBufferSize() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_VBV_BUFFER_SIZE, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair(var->minValue.int64Value, var->maxValue.int64Value);
}

void Plugin::AMD::EncoderH264::SetVBVBufferSize(uint64_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_VBV_BUFFER_SIZE, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %lld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint64_t Plugin::AMD::EncoderH264::GetVBVBufferSize() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_VBV_BUFFER_SIZE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetVBVBufferInitialFullness(double v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_INITIAL_VBV_BUFFER_FULLNESS, (int64_t)(v * 64));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %lf (%d), error %ls (code %d)",
			m_UniqueId, v, (uint8_t)(v * 64), m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

float Plugin::AMD::EncoderH264::GetInitialVBVBufferFullness() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_INITIAL_VBV_BUFFER_FULLNESS, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (e / 64.0f);
}

// Properties - Picture Control
void Plugin::AMD::EncoderH264::SetIDRPeriod(uint32_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_IDR_PERIOD, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	m_PeriodIDR = v;
}

uint32_t Plugin::AMD::EncoderH264::GetIDRPeriod() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_IDR_PERIOD, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	m_PeriodIDR = (uint32_t)e;
	return (uint32_t)e;
}

void Plugin::AMD::EncoderH264::SetHeaderInsertionSpacing(uint32_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_HEADER_INSERTION_SPACING, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint32_t Plugin::AMD::EncoderH264::GetHeaderInsertionSpacing() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_HEADER_INSERTION_SPACING, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint32_t)e;
}

void Plugin::AMD::EncoderH264::SetGOPAlignmentEnabled(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(L"EnableGOPAlignment", v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::IsGOPAlignmentEnabled() {
	AMFTRACECALL;

	bool e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(L"EnableGOPAlignment", &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetDeblockingFilterEnabled(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_DE_BLOCKING_FILTER, v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::IsDeblockingFilterEnabled() {
	AMFTRACECALL;

	bool e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_DE_BLOCKING_FILTER, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

uint8_t Plugin::AMD::EncoderH264::CapsBFramePattern() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_B_PIC_PATTERN, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return (uint8_t)var->maxValue.int64Value;
}

void Plugin::AMD::EncoderH264::SetBFramePattern(uint8_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_B_PIC_PATTERN, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %d, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	m_TimestampOffset = v;
}

uint8_t Plugin::AMD::EncoderH264::GetBFramePattern() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_B_PIC_PATTERN, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint8_t)e;
}

void Plugin::AMD::EncoderH264::SetBFrameDeltaQP(int8_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_QP_B, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %d, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

int8_t Plugin::AMD::EncoderH264::GetBFrameDeltaQP() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_B_PIC_DELTA_QP, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (int8_t)e;
}

void Plugin::AMD::EncoderH264::SetBFrameReferenceEnabled(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_B_REFERENCE_ENABLE, v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::IsBFrameReferenceEnabled() {
	AMFTRACECALL;

	bool e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_B_REFERENCE_ENABLE, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetBFrameReferenceDeltaQP(int8_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_REF_B_PIC_DELTA_QP, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %d, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

int8_t Plugin::AMD::EncoderH264::GetBFrameReferenceDeltaQP() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_REF_B_PIC_DELTA_QP, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (int8_t)e;
}

// Properties - Motion Estimation
void Plugin::AMD::EncoderH264::SetMotionEstimationQuarterPixelEnabled(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_MOTION_HALF_PIXEL, v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set mode to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::IsMotionEstimationQuarterPixelEnabled() {
	AMFTRACECALL;

	bool e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_MOTION_HALF_PIXEL, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetMotionEstimationHalfPixelEnabled(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_MOTION_QUARTERPIXEL, v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set mode to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::IsMotionEstimationHalfPixelEnabled() {
	AMFTRACECALL;

	bool e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_MOTION_QUARTERPIXEL, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

// Properties - Intra-Refresh
std::pair<uint32_t, uint32_t> Plugin::AMD::EncoderH264::CapsIntraRefreshNumMBsPerSlot() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_INTRA_REFRESH_NUM_MBS_PER_SLOT, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair((uint32_t)var->minValue.int64Value, (uint32_t)var->maxValue.int64Value);
}

void Plugin::AMD::EncoderH264::SetIntraRefreshNumMBsPerSlot(uint32_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_INTRA_REFRESH_NUM_MBS_PER_SLOT, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint32_t Plugin::AMD::EncoderH264::GetIntraRefreshNumMBsPerSlot() {
	AMFTRACECALL;

	int64_t e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_INTRA_REFRESH_NUM_MBS_PER_SLOT, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint32_t)e;
}

void Plugin::AMD::EncoderH264::SetIntraRefreshNumOfStripes(uint32_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(L"IntraRefreshNumOfStripes", (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint32_t Plugin::AMD::EncoderH264::GetIntraRefreshNumOfStripes() {
	AMFTRACECALL;

	int64_t e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(L"IntraRefreshNumOfStripes", &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint32_t)e;
}

// Properties - Slicing
void Plugin::AMD::EncoderH264::SetSliceMode(H264::SliceMode v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(L"SliceMode", static_cast<int64_t>(v));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::H264::SliceMode Plugin::AMD::EncoderH264::GetSliceMode() {
	AMFTRACECALL;

	int64_t e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(L"SliceMode", &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return static_cast<H264::SliceMode>(e);
}

std::pair<uint32_t, uint32_t> Plugin::AMD::EncoderH264::CapsSlicesPerFrame() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(AMF_VIDEO_ENCODER_SLICES_PER_FRAME, &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair((uint32_t)var->minValue.int64Value, (uint32_t)var->maxValue.int64Value);
}

void Plugin::AMD::EncoderH264::SetSlicesPerFrame(uint32_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(AMF_VIDEO_ENCODER_SLICES_PER_FRAME, (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint32_t Plugin::AMD::EncoderH264::GetSlicesPerFrame() {
	AMFTRACECALL;

	int64_t e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_SLICES_PER_FRAME, &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint32_t)e;
}

void Plugin::AMD::EncoderH264::SetSliceControlMode(SliceControlMode v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(L"SliceControlMode", static_cast<int64_t>(v));
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

Plugin::AMD::SliceControlMode Plugin::AMD::EncoderH264::GetSliceControlMode() {
	AMFTRACECALL;

	int64_t e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(L"SliceControlMode", &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return static_cast<SliceControlMode>(e);
}

std::pair<uint32_t, uint32_t> Plugin::AMD::EncoderH264::CapsSliceControlSize() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(L"SliceControlSize", &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair((uint32_t)var->minValue.int64Value, (uint32_t)var->maxValue.int64Value);
}

void Plugin::AMD::EncoderH264::SetSliceControlSize(uint32_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(L"SliceControlSize", (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint32_t Plugin::AMD::EncoderH264::GetSliceControlSize() {
	AMFTRACECALL;

	int64_t e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(L"SliceControlSize", &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint32_t)e;
}

std::pair<uint32_t, uint32_t> Plugin::AMD::EncoderH264::CapsMaximumSliceSize() {
	AMFTRACECALL;

	const amf::AMFPropertyInfo* var;
	AMF_RESULT res = m_AMFEncoder->GetPropertyInfo(L"MaxSliceSize", &var);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Querying capabilities failed, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}

	return std::make_pair((uint32_t)var->minValue.int64Value, (uint32_t)var->maxValue.int64Value);
}

void Plugin::AMD::EncoderH264::SetMaximumSliceSize(uint32_t v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(L"MaxSliceSize", (int64_t)v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set to %ld, error %ls (code %d)",
			m_UniqueId, v, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

uint32_t Plugin::AMD::EncoderH264::GetMaximumSliceSize() {
	AMFTRACECALL;

	int64_t e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(L"MaxSliceSize", &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return (uint32_t)e;
}

// Properties - Experimental
void Plugin::AMD::EncoderH264::SetLowLatencyInternal(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(L"LowLatencyInternal", v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set mode to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::GetLowLatencyInternal() {
	AMFTRACECALL;

	bool e;

	AMF_RESULT res = m_AMFEncoder->GetProperty(L"LowLatencyInternal", &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

void Plugin::AMD::EncoderH264::SetCommonLowLatencyInternal(bool v) {
	AMFTRACECALL;

	AMF_RESULT res = m_AMFEncoder->SetProperty(L"CommonLowLatencyInternal", v);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to set mode to %s, error %ls (code %d)",
			m_UniqueId, v ? "Enabled" : "Disabled", m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
}

bool Plugin::AMD::EncoderH264::GetCommonLowLatencyInternal() {
	AMFTRACECALL;

	bool e;
	AMF_RESULT res = m_AMFEncoder->GetProperty(L"CommonLowLatencyInternal", &e);
	if (res != AMF_OK) {
		QUICK_FORMAT_MESSAGE(errMsg, PREFIX "<" __FUNCTION_NAME__ "> Failed to retrieve value, error %ls (code %d)",
			m_UniqueId, m_AMF->GetTrace()->GetResultText(res), res);
		throw std::exception(errMsg.c_str());
	}
	return e;
}

// Internal
void Plugin::AMD::EncoderH264::PacketPriorityAndKeyframe(amf::AMFDataPtr& pData, struct encoder_packet* packet) {
	AMFTRACECALL;
	uint64_t pktType;
	pData->GetProperty(AMF_VIDEO_ENCODER_OUTPUT_DATA_TYPE, &pktType);
	switch ((AMF_VIDEO_ENCODER_OUTPUT_DATA_TYPE_ENUM)pktType) {
		case AMF_VIDEO_ENCODER_OUTPUT_DATA_TYPE_IDR:
			packet->keyframe = true;
		case AMF_VIDEO_ENCODER_OUTPUT_DATA_TYPE_I:
			packet->priority = 3;
			break;
		case AMF_VIDEO_ENCODER_OUTPUT_DATA_TYPE_P:
			packet->priority = 2;
			break;
		case AMF_VIDEO_ENCODER_OUTPUT_DATA_TYPE_B:
			packet->priority = 0;
			break;
	}
}

AMF_RESULT Plugin::AMD::EncoderH264::GetExtraDataInternal(amf::AMFVariant* p) {
	AMFTRACECALL;
	return m_AMFEncoder->GetProperty(AMF_VIDEO_ENCODER_EXTRADATA, p);
}

std::string Plugin::AMD::EncoderH264::HandleTypeOverride(amf::AMFSurfacePtr & d, uint64_t index) {
	AMF_VIDEO_ENCODER_PICTURE_TYPE_ENUM type = AMF_VIDEO_ENCODER_PICTURE_TYPE_NONE;

	if ((m_PeriodBFrame > 0) && ((index % m_PeriodBFrame) == 0)) {
		type = AMF_VIDEO_ENCODER_PICTURE_TYPE_B;
	}
	if ((m_PeriodPFrame > 0) && ((index % m_PeriodPFrame) == 0)) {
		type = AMF_VIDEO_ENCODER_PICTURE_TYPE_P;
	}
	if ((m_PeriodIFrame > 0) && ((index % m_PeriodIFrame) == 0)) {
		type = AMF_VIDEO_ENCODER_PICTURE_TYPE_I;
	}	
	if ((type != AMF_VIDEO_ENCODER_PICTURE_TYPE_NONE) && (m_PeriodIDR > 0) && ((index % m_PeriodIDR) == 0)) {
		type = AMF_VIDEO_ENCODER_PICTURE_TYPE_IDR;
	}
	if (m_FrameSkipPeriod > 0) {
		bool shouldSkip = m_FrameSkipKeepOnlyNth
			? (index % m_FrameSkipPeriod) != 0
			: (index % m_FrameSkipPeriod) == 0;

		if (shouldSkip) {
			if ((m_FrameSkipType <= AMF_VIDEO_ENCODER_PICTURE_TYPE_SKIP) || (type < m_FrameSkipType))
				m_FrameSkipType = type;
			type = AMF_VIDEO_ENCODER_PICTURE_TYPE_SKIP;
		} else if (m_FrameSkipType != AMF_VIDEO_ENCODER_PICTURE_TYPE_NONE) {
			type = m_FrameSkipType; // Hopefully fixes the crash.
			m_FrameSkipType = AMF_VIDEO_ENCODER_PICTURE_TYPE_NONE;
		}
	}
	if (type != AMF_VIDEO_ENCODER_PICTURE_TYPE_NONE)
		d->SetProperty(AMF_VIDEO_ENCODER_FORCE_PICTURE_TYPE, type);

	switch (type) {
		case AMF_VIDEO_ENCODER_PICTURE_TYPE_NONE:
			return "Automatic";
			break;
		case AMF_VIDEO_ENCODER_PICTURE_TYPE_SKIP:
			return "Skip";
			break;
		case AMF_VIDEO_ENCODER_PICTURE_TYPE_IDR:
			return "IDR";
			break;
		case AMF_VIDEO_ENCODER_PICTURE_TYPE_I:
			return "I";
			break;
		case AMF_VIDEO_ENCODER_PICTURE_TYPE_P:
			return "P";
			break;
		case AMF_VIDEO_ENCODER_PICTURE_TYPE_B:
			return "B";
			break;
	}
	return "Unknown";
}

void Plugin::AMD::EncoderH264::LogProperties() {
	AMFTRACECALL;

	PLOG_INFO(PREFIX "Encoder Parameters:",
		m_UniqueId);
	#pragma region Backend
	PLOG_INFO(PREFIX "  Backend:",
		m_UniqueId);
	PLOG_INFO(PREFIX "    Video API: %s",
		m_UniqueId,
		m_API->GetName().c_str());
	PLOG_INFO(PREFIX "    Video Adapter: %s",
		m_UniqueId,
		m_APIAdapter.Name.c_str());
	PLOG_INFO(PREFIX "    OpenCL: %s",
		m_UniqueId,
		m_OpenCL ? "Supported" : "Not Supported");
	PLOG_INFO(PREFIX "      Transfer: %s",
		m_UniqueId,
		m_OpenCLSubmission ? "Enabled" : "Disabled");
	PLOG_INFO(PREFIX "      Conversion: %s",
		m_UniqueId,
		m_OpenCLConversion ? "Enabled" : "Disabled");
	PLOG_INFO(PREFIX "    Multi-Threading: %s",
		m_UniqueId,
		m_MultiThreading ? "Enabled" : "Disabled");
	PLOG_INFO(PREFIX "    Queue Size: %" PRIu32,
		m_UniqueId,
		(uint32_t)GetQueueSize());
	#pragma endregion Backend
	#pragma region Frame
	PLOG_INFO(PREFIX "  Frame:",
		m_UniqueId);
	PLOG_INFO(PREFIX "    Format: %s %s %s",
		m_UniqueId,
		Utility::ColorFormatToString(m_ColorFormat),
		Utility::ColorSpaceToString(m_ColorSpace),
		m_FullColorRange ? "Full" : "Partial");
	PLOG_INFO(PREFIX "    Resolution: %" PRIu32 "x%" PRIu32,
		m_UniqueId,
		m_Resolution.first,
		m_Resolution.second);
	PLOG_INFO(PREFIX "    Frame Rate: %" PRIu32 "/%" PRIu32,
		m_UniqueId,
		m_FrameRate.first,
		m_FrameRate.second);
	auto aspectRatio = GetAspectRatio();
	PLOG_INFO(PREFIX "    Aspect Ratio: %" PRIu32 ":%" PRIu32,
		m_UniqueId,
		aspectRatio.first,
		aspectRatio.second);
	#pragma endregion Frame
	#pragma region Static
	PLOG_INFO(PREFIX "  Static:",
		m_UniqueId);
	PLOG_INFO(PREFIX "    Usage: %s",
		m_UniqueId,
		Utility::UsageToString(GetUsage()));
	PLOG_INFO(PREFIX "    Quality Preset: %s",
		m_UniqueId,
		Utility::QualityPresetToString(GetQualityPreset()));
	auto profileLevel = static_cast<uint16_t>(GetProfileLevel());
	PLOG_INFO(PREFIX "    Profile: %s %" PRIu16 ".%" PRIu16,
		m_UniqueId,
		Utility::ProfileToString(GetProfile()),
		profileLevel / 10,
		profileLevel % 10);
	PLOG_INFO(PREFIX "    Coding Type: %s",
		m_UniqueId,
		Utility::CodingTypeToString(GetCodingType()));
	PLOG_INFO(PREFIX "    Max. Reference Frames: %" PRIu16,
		m_UniqueId,
		(uint16_t)GetMaximumReferenceFrames());
	PLOG_INFO(PREFIX "    Max. Long-Term Reference Frames: %" PRIu16,
		m_UniqueId,
		(uint16_t)GetMaximumLongTermReferenceFrames());
	#pragma endregion Static
	#pragma region Rate Control
	PLOG_INFO(PREFIX "  Rate Control:",
		m_UniqueId);
	PLOG_INFO(PREFIX "    Method: %s",
		m_UniqueId,
		Utility::RateControlMethodToString(GetRateControlMethod()));
	PLOG_INFO(PREFIX "    Pre-Pass Mode: %s",
		m_UniqueId,
		Utility::PrePassModeToString(GetPrePassMode()));
	#pragma region QP
	PLOG_INFO(PREFIX "    QP:",
		m_UniqueId);
	PLOG_INFO(PREFIX "      Range: %" PRIu8 " - %" PRIu8,
		m_UniqueId,
		GetQPMinimum(),
		GetQPMaximum());
	PLOG_INFO(PREFIX "      I-Frame: %" PRIu8,
		m_UniqueId,
		GetIFrameQP());
	PLOG_INFO(PREFIX "      P-Frame: %" PRIu8,
		m_UniqueId,
		GetPFrameQP());
	try {
		PLOG_INFO(PREFIX "      B-Frame: %" PRIu8,
			m_UniqueId,
			GetBFrameQP());
	} catch (...) {
		PLOG_INFO(PREFIX "      B-Frame: N/A",
			m_UniqueId);
	}
	#pragma endregion QP
	#pragma region Bitrate
	PLOG_INFO(PREFIX "    Bitrate:",
		m_UniqueId);
	PLOG_INFO(PREFIX "      Target: %" PRIu64 " bit/s",
		m_UniqueId,
		GetTargetBitrate());
	PLOG_INFO(PREFIX "      Peak: %" PRIu64 " bit/s",
		m_UniqueId,
		GetPeakBitrate());
	#pragma endregion Bitrate
	#pragma region Flags
	PLOG_INFO(PREFIX "    Flags:",
		m_UniqueId);
	PLOG_INFO(PREFIX "      Filler Data: %s",
		m_UniqueId,
		IsFillerDataEnabled() ? "Enabled" : "Disabled");
	PLOG_INFO(PREFIX "      Frame Skipping: %s",
		m_UniqueId,
		IsFrameSkippingEnabled() ? "Enabled" : "Disabled");
	PLOG_INFO(PREFIX "        Period: %" PRIu32 " Frames",
		m_UniqueId,
		GetFrameSkippingPeriod());
	PLOG_INFO(PREFIX "        Behaviour: %s",
		m_UniqueId,
		GetFrameSkippingBehaviour() ? "Keep every Nth frame" : "Skip every Nth frame");
	PLOG_INFO(PREFIX "      Variance Based Adaptive Quantization: %s",
		m_UniqueId,
		IsVarianceBasedAdaptiveQuantizationEnabled() ? "Enabled" : "Disabled");
	PLOG_INFO(PREFIX "      Enforce Hypothetical Reference Decoder: %s",
		m_UniqueId,
		IsEnforceHRDEnabled() ? "Enabled" : "Disabled");
	#pragma endregion Flags
	#pragma region Video Buffering Verifier
	PLOG_INFO(PREFIX "    Video Buffering Verfier:",
		m_UniqueId);
	PLOG_INFO(PREFIX "      Buffer Size: %" PRIu64 " bits",
		m_UniqueId,
		GetVBVBufferSize());
	PLOG_INFO(PREFIX "      Initial Fullness: %" PRIu64 " %%",
		m_UniqueId,
		(uint64_t)round(GetInitialVBVBufferFullness() * 100.0));
	#pragma endregion Video Buffering Verifier
	PLOG_INFO(PREFIX "    Max. Access Unit Size: %" PRIu32,
		m_UniqueId,
		GetMaximumAccessUnitSize());
	#pragma endregion Rate Control

	#pragma region Picture Control
	PLOG_INFO(PREFIX "  Picture Control:",
		m_UniqueId);
	PLOG_INFO(PREFIX "    Period:",
		m_UniqueId);
	PLOG_INFO(PREFIX "      IDR: %" PRIu32 " Frames",
		m_UniqueId,
		GetIDRPeriod());
	PLOG_INFO(PREFIX "      I: %" PRIu32 " Frames",
		m_UniqueId,
		GetIFramePeriod());
	PLOG_INFO(PREFIX "      P: %" PRIu32 " Frames",
		m_UniqueId,
		GetPFramePeriod());
	PLOG_INFO(PREFIX "      B: %" PRIu32 " Frames",
		m_UniqueId,
		GetBFramePeriod());
	PLOG_INFO(PREFIX "    Header Insertion Spacing: %" PRIu32,
		m_UniqueId,
		GetHeaderInsertionSpacing());
	PLOG_INFO(PREFIX "    GOP Alignment: %s",
		m_UniqueId,
		IsGOPAlignmentEnabled() ? "Enabled" : "Disabled");
	PLOG_INFO(PREFIX "    Deblocking Filter: %s",
		m_UniqueId,
		IsDeblockingFilterEnabled() ? "Enabled" : "Disabled");
	PLOG_INFO(PREFIX "    Motion Estimation: %s%s",
		m_UniqueId,
		IsMotionEstimationQuarterPixelEnabled() ? (IsMotionEstimationHalfPixelEnabled() ? "Quarter, " : "Quarter") : "",
		IsMotionEstimationHalfPixelEnabled() ? "Half" : "");
	PLOG_INFO(PREFIX "    B-Frames:",
		m_UniqueId);
	try {
		PLOG_INFO(PREFIX "      Pattern: %" PRIu8,
			m_UniqueId,
			GetBFramePattern());
	} catch (...) {
		PLOG_INFO(PREFIX "      Pattern: N/A",
			m_UniqueId);
	}
	try {
		PLOG_INFO(PREFIX "      Delta QP: %" PRIi8,
			m_UniqueId,
			GetBFrameDeltaQP());
	} catch (...) {
		PLOG_INFO(PREFIX "      Delta QP: N/A",
			m_UniqueId);
	}
	try {
		PLOG_INFO(PREFIX "      Reference: %s",
			m_UniqueId,
			IsBFrameReferenceEnabled() ? "Enabled" : "Disabled");
	} catch (...) {
		PLOG_INFO(PREFIX "      Reference: N/A",
			m_UniqueId);
	}
	try {
		PLOG_INFO(PREFIX "      Reference Delta QP: %" PRIi8,
			m_UniqueId,
			GetBFrameReferenceDeltaQP());
	} catch (...) {
		PLOG_INFO(PREFIX "      Reference Delta QP: N/A",
			m_UniqueId);
	}
	#pragma endregion Picture Control

	#pragma region Intra-Refresh
	PLOG_INFO(PREFIX "  Intra-Refresh:",
		m_UniqueId);
	PLOG_INFO(PREFIX "    Number of Macroblocks Per Slot: %" PRIu32,
		m_UniqueId,
		GetIntraRefreshNumMBsPerSlot());
	PLOG_INFO(PREFIX "    Number of Stripes: %" PRIu32,
		m_UniqueId,
		GetIntraRefreshNumOfStripes());
	#pragma endregion Intra-Refresh

	#pragma region Slicing
	PLOG_INFO(PREFIX "  Slicing:",
		m_UniqueId);
	PLOG_INFO(PREFIX "    Mode: %" PRIu64,
		m_UniqueId,
		Utility::SliceModeToString(GetSliceMode()));
	PLOG_INFO(PREFIX "    Slices Per Frame: %" PRIu32,
		m_UniqueId,
		GetSlicesPerFrame());
	PLOG_INFO(PREFIX "    Control Mode: %s",
		m_UniqueId,
		Utility::SliceControlModeToString(GetSliceControlMode()));
	PLOG_INFO(PREFIX "    Control Size: %" PRIu32,
		m_UniqueId,
		GetSliceControlSize());
	PLOG_INFO(PREFIX "    Maximum Slice Size: %" PRIu32,
		m_UniqueId,
		GetMaximumSliceSize());
	#pragma endregion Slicing

	#pragma region Experimental
	PLOG_INFO(PREFIX "  Experimental:",
		m_UniqueId);
	PLOG_INFO(PREFIX "    Low Latency: %s",
		m_UniqueId,
		GetLowLatencyInternal() ? "Enabled" : "Disabled");
	PLOG_INFO(PREFIX "    Ultra Low Latency: %s",
		m_UniqueId,
		GetCommonLowLatencyInternal() ? "Enabled" : "Disabled");
	#pragma endregion Experimental

	//PLOG_INFO(PREFIX "  ");
	//PLOG_INFO(PREFIX "    ");
}
