<template>
	<view class="dev-area">
		
		<view class="dev-cart">
			<view class="">
				<view class="dev-name">灯</view>
			<image class="dev-logo" src="/static/light.png" mode=""></image>
			</view>
			<switch :checked="button" @change="onbuttonSwitch" color="#2b9932"/> 
			
		</view>
		
		<view class="">
			<view class="dev-name">连接状态：{{status}}</view>
		</view>

	</view>
</template>

<script>
	const{createCommonToken} = require('@/key.js')
	export default {
		data() {
			return {
				title: 'Hello',
				button: false,//改变button名
				status: 'No',
				token:'',
			}
		},
		onLoad() {
				const params = {
					author_key:'URtqTfo7Q7UXBRDmnN0UI5DPHgp7MaVbvalqa+R18/XJxxXI/6tJmXHiIJkjL7c3',//改变author_key
					version:'2022-05-01',
					user_id:'437237',//改变用户id
					
				}
				
				this.token = createCommonToken(params);
				
		},
		onShow() {
			this.fetchDevData();
			this.fetchDevStatus();//2s接收最新状态
			setInterval(()=>{
				this.fetchDevData();this.fetchDevStatus();
			},2000)
		},
		
		methods: {
			//获取最新Data
			fetchDevData(){
				uni.request({
				    url: 'https://iot-api.heclouds.com/thingmodel/query-device-property', //接口地址
					method:'GET',
				    data: {
						product_id:'0kk25wrig9',//产品id
						device_name:'test'//设备id
				    },
				    header: {
				        'authorization': this.token //自定义请求头信息
				    },
				    success: (res) => {
				        console.log(res.data);
						console.log(res.data.data[0].value);
						this.button=res.data.data[0].value === 'true'?true : false;
				    }
				});
			},
			//按键改变
			onbuttonSwitch(event) {
				console.log(event.detail.value);
				let value = event.detail.value;
				uni.request({
				    url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //接口地址
					method:'POST',
				    data: {
						product_id:'0kk25wrig9',//产品id
						device_name:'test',//设备id
						params: {"button": value}
				    },
				    header: {
				        'authorization': this.token //自定义请求头信息
				    },
				    success: (res) => {
				        console.log('led' + (value ? 'ON' : 'OFF') + '!');
						
				    }
				});
			},
			//获取当前OneNet连接状态
			fetchDevStatus(){
				let value = 'Yes';
				uni.request({
				    url: 'https://iot-api.heclouds.com/device/detail', //接口地址
					method:'GET',
				    data: {
						product_id:'0kk25wrig9',//产品id
						device_name:'test',//设备id
				    },
				    header: {
				        'authorization': this.token //自定义请求头信息
				    },
				    success: (res) => {
						 console.log(res.data.data.status ? 'Yes' : 'No');
						 this.status=(res.data.data.status ? 'Yes' : 'No');
						
				    }
				});
			}
			
		},
		
	}
</script>

<style>
	
	.dev-area {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
	}

	.dev-cart {
		height: 150rpx;
		width: 320rpx;
		border-radius: 150rpx;
		margin-top: 400rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		box-shadow: 0 0 15rpx #ccc;
	}
	.dev-name{
		font-size: 20rpx;
		color: #6d6d6d;
		text-align: center;
	}
	.dev-logo {
		width: 70rpx;
		height: 70rpx;
		margin-top: 10rpx;
	}
</style>
