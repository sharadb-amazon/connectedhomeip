#import "CastingViewController.h"
#import "CHIPUIViewUtils.h"
#include <arpa/inet.h>
 
#define COMMISSIONER_SERVICE_TYPE @"_chipd._udp"
#define COMMISSIONER_DOMAIN @""

#define COMMISSIONEE_SERVICE_TYPE @"_chipc._udp"
#define COMMISSIONEE_SERVICE_NAME @"CHIPTool (iOS)"
#define COMMISSIONEE_DOMAIN @""
#define COMMISSIONEE_PORT 4567
#define COMMISSIONEE_DN @"CHIPTool (iOS)"
#define COMMISSIONEE_DT @"81"
#define COMMISSIONEE_VP @"123"

@interface CastingViewController ()
@property(nonatomic, retain) NSNetServiceBrowser *serviceBrowser;
@property(nonatomic, retain) NSNetService *serviceResolver;
@property(nonatomic, retain) NSNetService *commissioneeService;
@property(nonatomic, retain) NSMutableArray* services;
@property (nonatomic, strong) UIStackView * stackView;
@end
 
@implementation CastingViewController
 
- (void)viewDidLoad
{
    [super viewDidLoad];
 
    NSLog(@"CastingViewController: viewDidLoad");
    
    self.services = [[NSMutableArray alloc] init];
    self.serviceBrowser = [[NSNetServiceBrowser alloc] init];
    self.serviceBrowser.delegate = self;
 
    [self bootstrapUI];
    [self startCommissionerDiscovery];
}

- (void)bootstrapUI
{
    self.view.backgroundColor = UIColor.whiteColor;
    
    // stack view
    _stackView = [UIStackView new];
    _stackView.axis = UILayoutConstraintAxisVertical;
    _stackView.distribution = UIStackViewDistributionEqualSpacing;
    _stackView.alignment = UIStackViewAlignmentLeading;
    _stackView.spacing = 30;
    [_stackView removeFromSuperview];
    [self.view addSubview:_stackView];
    [_stackView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:30].active = YES;
    [_stackView.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-30].active = YES;
    _stackView.translatesAutoresizingMaskIntoConstraints = false;

    // Title
    UILabel * titleLabel = [CHIPUIViewUtils addTitle:@"Casting" toView:self.view];
    [_stackView.topAnchor constraintEqualToAnchor:titleLabel.bottomAnchor constant:30].active = YES;
}
  
- (void)startCommissionerDiscovery
{
    NSLog(@"CastingViewController: startCommissionerDiscovery");
    
    UILabel * discoveringStatusLabel = [UILabel new];
    discoveringStatusLabel.text = @"Discovering commissioners...";
    [_stackView addArrangedSubview:discoveringStatusLabel];

    [self.serviceBrowser searchForServicesOfType:COMMISSIONER_SERVICE_TYPE inDomain:COMMISSIONER_DOMAIN];
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)service moreComing:(BOOL)moreComing
{
    NSLog(@"CastingViewController: didFindService %@", service);
    [self.services addObject:service];

    [service setDelegate:self];
    [service startMonitoring];
    [service resolveWithTimeout:30.0];
}

- (void)netServiceDidResolveAddress:(NSNetService *)service {
    NSLog(@"CastingViewController: netServiceDidResolveAddress");

    NSString * buttonText = [self getCommissionerButtonText:service];

    UIButton * commissionerButton = [UIButton new];
    [commissionerButton setTitle:buttonText forState:UIControlStateNormal];
    [commissionerButton addTarget:self action:@selector(commissionerSelected:) forControlEvents:UIControlEventTouchUpInside];
    commissionerButton.titleLabel.font = [UIFont systemFontOfSize:30];
    commissionerButton.titleLabel.textColor = [UIColor blackColor];
    commissionerButton.layer.cornerRadius = 5;
    commissionerButton.clipsToBounds = YES;
    commissionerButton.backgroundColor = UIColor.systemBlueColor;
    [_stackView addArrangedSubview:commissionerButton];
}

- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict {
    NSLog(@"CastingViewController: didNotResolve");
    [self.serviceResolver stop];
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)netServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreServicesComing
{
    NSLog(@"CastingViewController: didRemoveService");
    
    if (self.serviceResolver && [aNetService isEqual:self.serviceResolver]) {
        [self.serviceResolver stop];
    }
 
    [self.services removeObject:aNetService];
    if (!moreServicesComing) {
        [self.tableView reloadData];
    }
}

- (NSString *)getCommissionerButtonText:(NSNetService *)service
{
    NSData* data = [service TXTRecordData];
    NSDictionary* dict = [NSNetService dictionaryFromTXTRecordData:data];
    NSString* dn = [[NSString alloc] initWithData:[dict valueForKey:@"DN"] encoding:NSUTF8StringEncoding];
    NSString* dt = [[NSString alloc] initWithData:[dict valueForKey:@"DT"] encoding:NSUTF8StringEncoding];
    NSString* vp = [[NSString alloc] initWithData:[dict valueForKey:@"VP"] encoding:NSUTF8StringEncoding];
    NSLog(@"CastingViewController: getCommissionerButtonText dn: %@, dt: %@, vp: %@, service name: %@, service type: %@", dn, dt, vp, service.name, service.type);

    NSString * main = dn.length != 0 ? dn : @"";
    NSString * text = main;

    // text = [text stringByAppendingString:@" extra"];
    NSLog(@"CastingViewController: getCommissionerButtonText %@", text);
    return text;
}

- (IBAction)commissionerSelected:(id)sender
{
    UIButton * button = (UIButton *) sender;
    NSLog(@"CastingViewController: commissionerSelected: %@", button.titleLabel.text);
    [self publishCommissioneeOnPort:COMMISSIONEE_PORT];
    
    UILabel * pinMessageLabel = [UILabel new];
    pinMessageLabel.text = @"PIN";
    [_stackView addArrangedSubview:pinMessageLabel];
    
    UILabel * pinValueLabel = [UILabel new];
    pinValueLabel.text = @"X1Y2Z3";
    pinValueLabel.textAlignment = NSTextAlignmentCenter;
    pinValueLabel.font = [UIFont systemFontOfSize:30];
    [_stackView addArrangedSubview:pinValueLabel];


}

- (void)publishCommissioneeOnPort:(UInt16)port
{
    NSLog(@"CastingViewController: publishCommissioneeOnPort");
    _commissioneeService = [[NSNetService alloc] initWithDomain:COMMISSIONEE_DOMAIN type:COMMISSIONEE_SERVICE_TYPE name:COMMISSIONEE_SERVICE_NAME port:port];
    
    if(_commissioneeService)
    {
        [_commissioneeService setDelegate:self];
        [_commissioneeService publishWithOptions:NSNetServiceNoAutoRename];
    }
    else
    {
        NSLog(@"An error occurred initializing the NSNetService object.");
    }
    
    NSMutableDictionary *mDict = [NSMutableDictionary dictionaryWithCapacity:8];
    [mDict setObject:COMMISSIONEE_DN                forKey:@"DN"];
    [mDict setObject:COMMISSIONEE_DT            forKey:@"DT"];
    [mDict setObject:COMMISSIONEE_VP           forKey:@"VP"];
    [_commissioneeService setTXTRecordData: [NSNetService dataFromTXTRecordDictionary:mDict]];
}
 
- (void)netServiceWillPublish:(NSNetService *)sender
{
    NSLog(@"netServiceWillPublish");
}

- (void)netServiceDidPublish:(NSNetService *)sender
{
    NSLog(@"netServiceDidPublish");
}

- (void)netService:(NSNetService *)sender didNotPublish:(NSDictionary *)errorDict
{
    NSLog(@"netService:didNotPublish: %@", errorDict);
}

@end
