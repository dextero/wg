#include <stdio.h>
#import <Foundation/Foundation.h>

const char * CreateDirectoryIfNotExists(const char *dname)
{
	id path = [[NSString stringWithUTF8String:dname] stringByReplacingOccurrencesOfString:@"~/" withString:@""];
	path = [NSHomeDirectory() stringByAppendingPathComponent:path];
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSLog(@"OSX: User preferences directory: %@\n", path);
	if (![fileManager fileExistsAtPath:path])
	{
		NSLog(@"OSX: Creating user preferences directory...\n");
		[fileManager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:0];
	}
	return [path UTF8String];
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