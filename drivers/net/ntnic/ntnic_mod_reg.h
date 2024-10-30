/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2023 Napatech A/S
 */

#ifndef __NTNIC_MOD_REG_H__
#define __NTNIC_MOD_REG_H__

#include <stdint.h>

#include "rte_ethdev.h"
#include "rte_flow_driver.h"

#include "flow_api.h"
#include "stream_binary_flow_api.h"
#include "nthw_fpga_model.h"
#include "nthw_platform_drv.h"
#include "nthw_drv.h"
#include "nt4ga_adapter.h"
#include "ntnic_nthw_fpga_rst_nt200a0x.h"
#include "ntnic_virt_queue.h"
#include "create_elements.h"

/* sg ops section */
struct sg_ops_s {
	/* Setup a virtQueue for a VM */
	struct nthw_virt_queue *(*nthw_setup_rx_virt_queue)(nthw_dbs_t *p_nthw_dbs,
		uint32_t index,
		uint16_t start_idx,
		uint16_t start_ptr,
		void *avail_struct_phys_addr,
		void *used_struct_phys_addr,
		void *desc_struct_phys_addr,
		uint16_t queue_size,
		uint32_t host_id,
		uint32_t header,
		uint32_t vq_type,
		int irq_vector);
	struct nthw_virt_queue *(*nthw_setup_tx_virt_queue)(nthw_dbs_t *p_nthw_dbs,
		uint32_t index,
		uint16_t start_idx,
		uint16_t start_ptr,
		void *avail_struct_phys_addr,
		void *used_struct_phys_addr,
		void *desc_struct_phys_addr,
		uint16_t queue_size,
		uint32_t host_id,
		uint32_t port,
		uint32_t virtual_port,
		uint32_t header,
		uint32_t vq_type,
		int irq_vector,
		uint32_t in_order);
	struct nthw_virt_queue *(*nthw_setup_mngd_rx_virt_queue)(nthw_dbs_t *p_nthw_dbs,
		uint32_t index,
		uint32_t queue_size,
		uint32_t host_id,
		uint32_t header,
		/*
		 * Memory that can be used
		 * for virtQueue structs
		 */
		struct nthw_memory_descriptor *p_virt_struct_area,
		/*
		 * Memory that can be used for packet
		 * buffers - Array must have queue_size
		 * entries
		 */
		struct nthw_memory_descriptor *p_packet_buffers,
		uint32_t vq_type,
		int irq_vector);
	int (*nthw_release_mngd_rx_virt_queue)(struct nthw_virt_queue *rxvq);
	struct nthw_virt_queue *(*nthw_setup_mngd_tx_virt_queue)(nthw_dbs_t *p_nthw_dbs,
		uint32_t index,
		uint32_t queue_size,
		uint32_t host_id,
		uint32_t port,
		uint32_t virtual_port,
		uint32_t header,
		/*
		 * Memory that can be used
		 * for virtQueue structs
		 */
		struct nthw_memory_descriptor *p_virt_struct_area,
		/*
		 * Memory that can be used for packet
		 * buffers - Array must have queue_size
		 * entries
		 */
		struct nthw_memory_descriptor *p_packet_buffers,
		uint32_t vq_type,
		int irq_vector,
		uint32_t in_order);
	int (*nthw_release_mngd_tx_virt_queue)(struct nthw_virt_queue *txvq);
	/*
	 * These functions handles both Split and Packed including merged buffers (jumbo)
	 */
	uint16_t (*nthw_get_rx_packets)(struct nthw_virt_queue *rxvq,
		uint16_t n,
		struct nthw_received_packets *rp,
		uint16_t *nb_pkts);
	void (*nthw_release_rx_packets)(struct nthw_virt_queue *rxvq, uint16_t n);
	uint16_t (*nthw_get_tx_packets)(struct nthw_virt_queue *txvq,
		uint16_t n,
		uint16_t *first_idx,
		struct nthw_cvirtq_desc *cvq,
		struct nthw_memory_descriptor **p_virt_addr);
	void (*nthw_release_tx_packets)(struct nthw_virt_queue *txvq,
		uint16_t n,
		uint16_t n_segs[]);
	int (*nthw_virt_queue_init)(struct fpga_info_s *p_fpga_info);
};

void register_sg_ops(struct sg_ops_s *ops);
const struct sg_ops_s *get_sg_ops(void);
void sg_init(void);

struct ntnic_filter_ops {
	int (*poll_statistics)(struct pmd_internals *internals);
};

void register_ntnic_filter_ops(const struct ntnic_filter_ops *ops);
const struct ntnic_filter_ops *get_ntnic_filter_ops(void);
void ntnic_filter_init(void);

struct link_ops_s {
	int (*link_init)(struct adapter_info_s *p_adapter_info, nthw_fpga_t *p_fpga);
};

void register_100g_link_ops(struct link_ops_s *ops);
const struct link_ops_s *get_100g_link_ops(void);
void link_100g_init(void);

struct port_ops {
	bool (*get_nim_present)(struct adapter_info_s *p, int port);

	/*
	 * port:s link mode
	 */
	void (*set_adm_state)(struct adapter_info_s *p, int port, bool adm_state);
	bool (*get_adm_state)(struct adapter_info_s *p, int port);

	/*
	 * port:s link status
	 */
	void (*set_link_status)(struct adapter_info_s *p, int port, bool status);
	bool (*get_link_status)(struct adapter_info_s *p, int port);

	/*
	 * port: link autoneg
	 */
	void (*set_link_autoneg)(struct adapter_info_s *p, int port, bool autoneg);
	bool (*get_link_autoneg)(struct adapter_info_s *p, int port);

	/*
	 * port: link speed
	 */
	void (*set_link_speed)(struct adapter_info_s *p, int port, nt_link_speed_t speed);
	nt_link_speed_t (*get_link_speed)(struct adapter_info_s *p, int port);

	/*
	 * port: link duplex
	 */
	void (*set_link_duplex)(struct adapter_info_s *p, int port, nt_link_duplex_t duplex);
	nt_link_duplex_t (*get_link_duplex)(struct adapter_info_s *p, int port);

	/*
	 * port: loopback mode
	 */
	void (*set_loopback_mode)(struct adapter_info_s *p, int port, uint32_t mode);
	uint32_t (*get_loopback_mode)(struct adapter_info_s *p, int port);

	uint32_t (*get_link_speed_capabilities)(struct adapter_info_s *p, int port);

	/*
	 * port: nim capabilities
	 */
	nim_i2c_ctx_t (*get_nim_capabilities)(struct adapter_info_s *p, int port);

	/*
	 * port: tx power
	 */
	int (*tx_power)(struct adapter_info_s *p, int port, bool disable);
};

void register_port_ops(const struct port_ops *ops);
const struct port_ops *get_port_ops(void);
void port_init(void);

struct nt4ga_stat_ops {
	int (*nt4ga_stat_init)(struct adapter_info_s *p_adapter_info);
	int (*nt4ga_stat_setup)(struct adapter_info_s *p_adapter_info);
	int (*nt4ga_stat_collect)(struct adapter_info_s *p_adapter_info,
		nt4ga_stat_t *p_nt4ga_stat);
};

void register_nt4ga_stat_ops(const struct nt4ga_stat_ops *ops);
const struct nt4ga_stat_ops *get_nt4ga_stat_ops(void);
void nt4ga_stat_ops_init(void);

struct adapter_ops {
	int (*init)(struct adapter_info_s *p_adapter_info);
	int (*deinit)(struct adapter_info_s *p_adapter_info);

	int (*show_info)(struct adapter_info_s *p_adapter_info, FILE *pfh);
};

void register_adapter_ops(const struct adapter_ops *ops);
const struct adapter_ops *get_adapter_ops(void);
void adapter_init(void);

struct clk9563_ops {
	const int *(*get_n_data_9563_si5340_nt200a02_u23_v5)(void);
	const clk_profile_data_fmt2_t *(*get_p_data_9563_si5340_nt200a02_u23_v5)(void);
};

void register_clk9563_ops(struct clk9563_ops *ops);
struct clk9563_ops *get_clk9563_ops(void);
void clk9563_ops_init(void);

struct rst_nt200a0x_ops {
	int (*nthw_fpga_rst_nt200a0x_init)(struct fpga_info_s *p_fpga_info,
		struct nthw_fpga_rst_nt200a0x *p_rst);
	int (*nthw_fpga_rst_nt200a0x_reset)(nthw_fpga_t *p_fpga,
		const struct nthw_fpga_rst_nt200a0x *p);
};

void register_rst_nt200a0x_ops(struct rst_nt200a0x_ops *ops);
struct rst_nt200a0x_ops *get_rst_nt200a0x_ops(void);
void rst_nt200a0x_ops_init(void);

struct rst9563_ops {
	int (*nthw_fpga_rst9563_init)(struct fpga_info_s *p_fpga_info,
		struct nthw_fpga_rst_nt200a0x *const p);
};

void register_rst9563_ops(struct rst9563_ops *ops);
struct rst9563_ops *get_rst9563_ops(void);
void rst9563_ops_init(void);

struct flow_backend_ops {
	const struct flow_api_backend_ops *(*bin_flow_backend_init)(nthw_fpga_t *p_fpga,
		void **be_dev);
	void (*bin_flow_backend_done)(void *be_dev);
};

void register_flow_backend_ops(const struct flow_backend_ops *ops);
const struct flow_backend_ops *get_flow_backend_ops(void);
void flow_backend_init(void);

struct profile_inline_ops {
	/*
	 * Management
	 */

	int (*done_flow_management_of_ndev_profile_inline)(struct flow_nic_dev *ndev);

	int (*initialize_flow_management_of_ndev_profile_inline)(struct flow_nic_dev *ndev);

	/*
	 * Flow functionality
	 */
	int (*flow_destroy_locked_profile_inline)(struct flow_eth_dev *dev,
		struct flow_handle *fh,
		struct rte_flow_error *error);

	struct flow_handle *(*flow_create_profile_inline)(struct flow_eth_dev *dev,
		const struct rte_flow_attr *attr,
		uint16_t forced_vlan_vid,
		uint16_t caller_id,
		const struct rte_flow_item elem[],
		const struct rte_flow_action action[],
		struct rte_flow_error *error);

	int (*flow_destroy_profile_inline)(struct flow_eth_dev *dev,
		struct flow_handle *flow,
		struct rte_flow_error *error);

	int (*flow_flush_profile_inline)(struct flow_eth_dev *dev,
		uint16_t caller_id,
		struct rte_flow_error *error);

	int (*flow_dev_dump_profile_inline)(struct flow_eth_dev *dev,
		struct flow_handle *flow,
		uint16_t caller_id,
		FILE *file,
		struct rte_flow_error *error);

	int (*flow_get_aged_flows_profile_inline)(struct flow_eth_dev *dev,
		uint16_t caller_id,
		void **context,
		uint32_t nb_contexts,
		struct rte_flow_error *error);

	int (*flow_nic_set_hasher_fields_inline)(struct flow_nic_dev *ndev,
		int hsh_idx,
		struct nt_eth_rss_conf rss_conf);

	/*
	 * Stats
	 */
	int (*flow_get_flm_stats_profile_inline)(struct flow_nic_dev *ndev,
		uint64_t *data,
		uint64_t size);

	/*
	 * NT Flow FLM queue API
	 */
	void (*flm_setup_queues)(void);
	void (*flm_free_queues)(void);
	uint32_t (*flm_update)(struct flow_eth_dev *dev);

	int (*flow_info_get_profile_inline)(struct flow_eth_dev *dev, uint8_t caller_id,
		struct rte_flow_port_info *port_info, struct rte_flow_queue_info *queue_info,
		struct rte_flow_error *error);

	int (*flow_configure_profile_inline)(struct flow_eth_dev *dev, uint8_t caller_id,
	const struct rte_flow_port_attr *port_attr, uint16_t nb_queue,
	const struct rte_flow_queue_attr *queue_attr[],
	struct rte_flow_error *error);
};

void register_profile_inline_ops(const struct profile_inline_ops *ops);
const struct profile_inline_ops *get_profile_inline_ops(void);
void profile_inline_init(void);

struct flow_filter_ops {
	int (*flow_filter_init)(nthw_fpga_t *p_fpga, struct flow_nic_dev **p_flow_device,
		int adapter_no);
	int (*flow_filter_done)(struct flow_nic_dev *dev);
	/*
	 * Device Management API
	 */
	struct flow_eth_dev *(*flow_get_eth_dev)(uint8_t adapter_no,
		uint8_t hw_port_no,
		uint32_t port_id,
		int alloc_rx_queues,
		struct flow_queue_id_s queue_ids[],
		int *rss_target_id,
		enum flow_eth_dev_profile flow_profile,
		uint32_t exception_path);
	int (*flow_dev_dump)(struct flow_eth_dev *dev,
		struct flow_handle *flow,
		uint16_t caller_id,
		FILE *file,
		struct rte_flow_error *error);
	/*
	 * NT Flow API
	 */
	struct flow_handle *(*flow_create)(struct flow_eth_dev *dev,
		const struct rte_flow_attr *attr,
		uint16_t forced_vlan_vid,
		uint16_t caller_id,
		const struct rte_flow_item item[],
		const struct rte_flow_action action[],
		struct rte_flow_error *error);

	int (*flow_destroy)(struct flow_eth_dev *dev,
		struct flow_handle *flow,
		struct rte_flow_error *error);

	int (*flow_flush)(struct flow_eth_dev *dev, uint16_t caller_id,
		struct rte_flow_error *error);

	int (*flow_get_flm_stats)(struct flow_nic_dev *ndev, uint64_t *data, uint64_t size);

	/*
	 * Other
	 */
	int (*flow_nic_set_hasher_fields)(struct flow_nic_dev *ndev, int hsh_idx,
		struct nt_eth_rss_conf rss_conf);
	int (*hw_mod_hsh_rcp_flush)(struct flow_api_backend_s *be, int start_idx, int count);

	int (*flow_get_aged_flows)(struct flow_eth_dev *dev,
		uint16_t caller_id,
		void **context,
		uint32_t nb_contexts,
		struct rte_flow_error *error);

	int (*flow_info_get)(struct flow_eth_dev *dev, uint8_t caller_id,
		struct rte_flow_port_info *port_info, struct rte_flow_queue_info *queue_info,
		struct rte_flow_error *error);

	int (*flow_configure)(struct flow_eth_dev *dev, uint8_t caller_id,
		const struct rte_flow_port_attr *port_attr, uint16_t nb_queue,
		const struct rte_flow_queue_attr *queue_attr[],
		struct rte_flow_error *error);
};

void register_dev_flow_ops(const struct rte_flow_ops *ops);
const struct rte_flow_ops *get_dev_flow_ops(void);
void dev_flow_init(void);

void register_flow_filter_ops(const struct flow_filter_ops *ops);
const struct flow_filter_ops *get_flow_filter_ops(void);
void init_flow_filter(void);

struct ntnic_xstats_ops {
	int (*nthw_xstats_get_names)(nt4ga_stat_t *p_nt4ga_stat,
		struct rte_eth_xstat_name *xstats_names,
		unsigned int size);
	int (*nthw_xstats_get)(nt4ga_stat_t *p_nt4ga_stat,
		struct rte_eth_xstat *stats,
		unsigned int n,
		uint8_t port);
	void (*nthw_xstats_reset)(nt4ga_stat_t *p_nt4ga_stat, uint8_t port);
	int (*nthw_xstats_get_names_by_id)(nt4ga_stat_t *p_nt4ga_stat,
		struct rte_eth_xstat_name *xstats_names,
		const uint64_t *ids,
		unsigned int size);
	int (*nthw_xstats_get_by_id)(nt4ga_stat_t *p_nt4ga_stat,
		const uint64_t *ids,
		uint64_t *values,
		unsigned int n,
		uint8_t port);
};

void register_ntnic_xstats_ops(struct ntnic_xstats_ops *ops);
struct ntnic_xstats_ops *get_ntnic_xstats_ops(void);
void ntnic_xstats_ops_init(void);

#endif	/* __NTNIC_MOD_REG_H__ */
