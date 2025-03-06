---
author: "Vidge Wong"
title: "AP - Persistency"
date: "2023-06-19"
summary: "Understanding Persistency in Adaptive AUTOSAR - mechanisms for storing information in non-volatile memory"
tags: ["vehicle"]
categories: ["AutoSAR"]
image: images/cover/default.jpg
hiddenInList: true
ShowToc: true
TocOpen: true
---

## Abbreviations

- **NvM**: Non-volatile Memory
- **CM**: Communication Management
- **DCM**: Diagnostic Communication Manager
- **EM**: Execution Management
- **URI**: Uniform Resource Identifier

## Overview

Persistency provides applications and Functional Clusters (FC) with a mechanism to store information in non-volatile memory (NvM), ensuring data is retained across restarts and ignition cycles. It offers a standardized interface for accessing NvM.

Applications specify storage location identifiers as parameters to the Persistency API to determine different storage locations. These storage locations are categorized into two types:

- **Key-Value Storage**
- **File Storage**

Applications can utilize a combination of these storage types as needed.

Persistent data is **process-private**. Data sharing between different processes through Persistency is not allowed to prevent communication pathways outside of Communication Management (CM).

## Key-Value Storage

Key-Value Storage offers a mechanism to access multiple key-value pairs from a single storage location. It supports the following data types:

- Data types defined in SWS_AdaptivePlatformTypes
- Simple byte arrays: streams of complex types
- All Implementation Data Types referenced by "dataTypeForSerialization" in "PersistencyKeyValueDatabaseInterface"

Each key in the key-value database must be unique and is defined through interfaces provided by Persistency.

## File Storage

Not all data is suitable for storage in key-value databases. For such data, **File Storage** is introduced. The File Storage Port allows applications to access a storage location and create one or more **Accessors** within it. These accessors are identified by unique keys in string format.

For better understanding, consider an analogy to a file system: a File Storage Port can be seen as a directory where applications can create multiple files (Accessors).

## UCM Use Cases for Persistent Data Handling

UCM supports three main scenarios for adaptive applications within the ECU or Adaptive Machine lifecycle:

1. Installing new application software
2. Upgrading existing application software
3. Uninstalling existing application software

Persistency needs to support the following scenarios:

- Deploying persistent data to key-value databases or file storage during adaptive application (AA) installation
- Deploying persistent data with modifications by the integrator
- Deploying persistent data as defined by the integrator
- Handling existing persistent data according to the configured upgrade strategy during application updates

The Persistency layer is configured during the application design and deployment phases. Deployment phase configurations can override application design configurations.

*Note: This article is based on AUTOSAR Adaptive Platform official document R20-11 version AUTOSAR_EXP_PlatformDesign.pdf.* 