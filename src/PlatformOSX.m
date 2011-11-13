#include <stdio.h>
#import <Foundation/Foundation.h>

const char * GetUserDir_OSX()
{
	id path = NSHomeDirectory();
	return [path UTF8String];
}

void CreateDir_OSX(const char * path)
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSLog(@"OSX: Creating directory `%s'...\n", path);
	[fileManager createDirectoryAtPath:[NSString stringWithUTF8String:path] withIntermediateDirectories:YES attributes:nil error:0];
} 

int AskForFullscreen_OSX(const wchar_t * title, const wchar_t * message, int maxw, int maxh)
{
	CFOptionFlags result;
	
	CFStringRef titleStr= CFStringCreateWithBytes (0, (void*)title, wcslen(title) * sizeof(wchar_t), kCFStringEncodingUTF32LE, false);
	CFStringRef messageStr= CFStringCreateWithBytes (0, (void*)message, wcslen(message) * sizeof(wchar_t), kCFStringEncodingUTF32LE, false);
								
	id messageStr2 = [NSString stringWithFormat:(id)messageStr, maxw, maxh];
	
	CFUserNotificationDisplayAlert(0, kCFUserNotificationCautionAlertLevel, 0, 0, 0, (CFStringRef)titleStr, (CFStringRef)messageStr2, (CFStringRef)@"OK", (CFStringRef)@"Cancel", nil, &result);
	
	if (result == 0) return 1; // OK
	else             return 0; // Cancel
}