#ifndef NET_H
#define NET_H

#include <include/list.h>
#include <kernel/fs/vfs.h>
#include <kernel/net/ip.h>
#include <kernel/net/arp.h>
#include <kernel/net/ethernet.h>

#define AF_INET 2 /* Internet IP Protocol 	*/
#define PF_INET AF_INET
#define AF_PACKET 17 /* Packet family		*/
#define PF_PACKET AF_PACKET

#define WORD_SIZE 4
#define WORD_MASK (~(WORD_SIZE - 1))
#define WORD_ALIGN(addr) (((addr) + WORD_SIZE - 1) & WORD_MASK)

typedef unsigned short sa_family_t;

struct sockaddr
{
  sa_family_t sa_family; /* address family, AF_xxx	*/
  char sa_data[14];      /* 14 bytes of protocol address	*/
};

/* Internet address. */
struct in_addr
{
  uint32_t s_addr; /* address in network byte order */
};

struct sockaddr_in
{
  sa_family_t sin_family;  /* address family: AF_INET */
  uint16_t sin_port;       /* port in network byte order */
  struct in_addr sin_addr; /* internet address */
};

#define PACKET_HOST 0      /* To us		*/
#define PACKET_BROADCAST 1 /* To all		*/
#define PACKET_MULTICAST 2 /* To group		*/
#define PACKET_OTHERHOST 3 /* To someone else 	*/
#define PACKET_OUTGOING 4  /* Outgoing of any type */
#define PACKET_LOOPBACK 5  /* MC/BRD frame looped back */
#define PACKET_USER 6      /* To user space	*/
#define PACKET_KERNEL 7    /* To kernel space	*/
/* Unused, PACKET_FASTROUTE and PACKET_LOOPBACK are invisible to user space */
#define PACKET_FASTROUTE 6 /* Fastrouted frame	*/

struct sockaddr_ll
{
  unsigned short sll_family;   /* Always AF_PACKET */
  unsigned short sll_protocol; /* Physical-layer protocol */
  int sll_ifindex;             /* Interface number */
  unsigned short sll_hatype;   /* ARP hardware type */
  unsigned char sll_pkttype;   /* Packet type */
  unsigned char sll_halen;     /* Length of address */
  unsigned char sll_addr[8];   /* Physical-layer address */
};

enum socket_state
{
  SS_FREE = 0,     /* not allocated		*/
  SS_UNCONNECTED,  /* unconnected to any socket	*/
  SS_CONNECTING,   /* in process of connecting	*/
  SS_CONNECTED,    /* connected to socket		*/
  SS_DISCONNECTING /* in process of disconnecting	*/
} socket_state;

enum socket_type
{
  SOCK_STREAM = 1,
  SOCK_DGRAM = 2,
  SOCK_RAW = 3,
};

struct socket
{
  enum socket_type type;
  enum socket_state state;
  unsigned long flags;
  struct proto_ops *ops;
  struct file *file;

  struct list_head tx_queue;
  struct list_head rx_queue;
  struct list_head sibling;

  // inet
  struct net_device *dev;
  uint32_t daddr;
  uint16_t dport;
  uint32_t saddr;
  uint16_t sport;
};

struct proto_ops
{
  int family;
  int (*release)(struct socket *sock);
  int (*bind)(struct socket *sock,
              struct sockaddr *myaddr,
              int sockaddr_len);
  int (*connect)(struct socket *sock,
                 struct sockaddr *vaddr,
                 int sockaddr_len);
  int (*accept)(struct socket *sock,
                struct socket *newsock);
  int (*getname)(struct socket *sock,
                 struct sockaddr *addr,
                 int *sockaddr_len, int peer);
  int (*listen)(struct socket *sock, int len);
  int (*shutdown)(struct socket *sock, int flags);
  int (*sendmsg)(struct socket *sock, char *msg, size_t msg_len);
  int (*recvmsg)(struct socket *sock, char *msg, size_t msg_len);
};

struct socket_alloc
{
  struct socket socket;
  struct vfs_inode inode;
};

static inline struct socket *SOCKET_I(struct vfs_inode *inode)
{
  return &container_of(inode, struct socket_alloc, inode)->socket;
}

static inline struct vfs_inode *SOCK_INODE(struct socket *socket)
{
  return &container_of(socket, struct socket_alloc, socket)->inode;
}

enum netdev_state
{
  NETDEV_STATE_OFF = 1,
  NETDEV_STATE_UP = 1 << 1,
  NETDEV_STATE_CONNECTED = 1 << 2, // interface connects and gets config (dhcp -> ip) from router
};

struct net_device
{
  uint32_t base_addr;
  uint32_t irq;

  char name[16];
  uint8_t state;
  struct list_head sibling;

  uint8_t dev_addr[6];
  uint8_t broadcast[6];
  uint32_t gateway;
  uint32_t ip;
};

struct sk_buff
{
  struct socket *sk;
  struct net_device *dev;
  struct list_head sibling;
  uint32_t len, true_size;

  union {
    struct udp_packet *udph;
    struct icmp_packet *icmph;
    uint8_t *raw;
  } h;

  union {
    struct ip4_packet *iph;
    struct arp_packet *arph;
    uint8_t *raw;
  } nh;

  union {
    struct ethernet_packet *eh;
    uint8_t *raw;
  } mac;

  uint8_t *head;
  uint8_t *data;
  uint8_t *tail;
  uint8_t *end;
};

static inline void skb_put(struct sk_buff *skb, uint32_t len)
{
  skb->tail += len;
  skb->len += len;
};

static inline void skb_push(struct sk_buff *skb, uint32_t len)
{
  skb->data -= len;
  skb->len += len;
}

static inline void skb_pull(struct sk_buff *skb, uint32_t len)
{
  skb->data += len;
  skb->len -= len;
}

void inet_init();
void push_rx_queue(uint8_t *data, uint32_t size);
void socket_setup(int32_t family, enum socket_type type, int32_t protocal, struct vfs_file *file);
struct socket *sockfd_lookup(uint32_t fd);
struct sk_buff *alloc_skb(uint32_t header_size, uint32_t payload_size);
uint16_t singular_checksum(void *packet, uint16_t size);
uint32_t packet_checksum_start(void *packet, uint16_t size);

void register_net_device(struct net_device *);
struct net_device *get_current_net_device();

// udp.c
extern struct proto_ops udp_proto_ops;

// raw.c
extern struct proto_ops raw_proto_ops;

#endif